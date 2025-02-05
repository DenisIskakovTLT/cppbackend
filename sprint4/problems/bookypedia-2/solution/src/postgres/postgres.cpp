#include "postgres.h"

#include <pqxx/zview.hxx>
#include <pqxx/pqxx>
#include <string>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void AuthorRepositoryImpl::Save(const domain::Author& author) {
    // Пока каждое обращение к репозиторию выполняется внутри отдельной транзакции
    // В будущих уроках вы узнаете про паттерн Unit of Work, при помощи которого сможете несколько
    // запросов выполнить в рамках одной транзакции.
    // Вы также может самостоятельно почитать информацию про этот паттерн и применить его здесь.
    pqxx::work work{connection_};
    work.exec_params(
        R"(
INSERT INTO authors (id, name) VALUES ($1, $2)
ON CONFLICT (id) DO UPDATE SET name=$2;
)"_zv,
        author.GetId().ToString(), author.GetName());
    work.commit();
}

void AuthorRepositoryImpl::Delete(const domain::AuthorId& author_id) {
    pqxx::work work{ connection_ , "serializable" };

    try {
        work.exec_params(
            R"(
            DELETE FROM book_tags
            WHERE book_id IN (
                SELECT id FROM books WHERE author_id = $1
            );
            )", author_id.ToString()
        );

        work.exec_params(
            R"(
            DELETE FROM books WHERE author_id = $1;
            )", author_id.ToString()
        );

        auto result = work.exec_params(
            R"(
            DELETE FROM authors WHERE id = $1;
            )", author_id.ToString()
        );

        if (result.affected_rows() == 0) {
            throw std::runtime_error("Author not found");
        }

        work.commit();
    }
    catch (const std::exception& e) {
        work.abort();
        throw std::runtime_error("Failed to delete author");
    }
}

void AuthorRepositoryImpl::Edit(const domain::AuthorId& author_id, const std::string& new_name) {
    pqxx::work work{ connection_ , "serializable" };

    try {
        auto result = work.exec_params(
            "UPDATE authors SET name = $2 WHERE id = $1",
            author_id.ToString(), new_name
        );

        if (result.affected_rows() == 0) {
            throw std::runtime_error("Author not found");
        }

        work.commit();
    }
    catch (const std::exception& e) {
        work.abort();
        throw std::runtime_error("Failed to edit author");
    }
}

void BookRepositoryImpl::Save(const domain::Book& book) {
    pqxx::work work{ connection_ , "serializable" };
    try {
        work.exec_params(
            "INSERT INTO books (id, title, author_id, publication_year) VALUES ($1, $2, $3, $4)",
            book.GetBookId().ToString(), book.GetTitle(), book.GetAuthorId().ToString(), book.GetPublicationYear());

        auto tags = book.GetTags();

        if (!tags.empty()) {
            for (const auto& tag : tags) {
                work.exec_params(
                    "INSERT INTO book_tags (book_id, tag) VALUES ($1, $2)",
                    book.GetBookId().ToString(), tag);
            }
        }

        work.commit();
    }
    catch (const std::exception& e) {
        work.abort();
        throw std::runtime_error("Failed to add book");
    }
}

std::vector<std::pair<domain::Book, std::string>> BookRepositoryImpl::GetBooks() {
    pqxx::work work{ connection_ , "serializable" };
    auto result = work.exec(
        R"(
        SELECT b.id, b.title, b.author_id, b.publication_year, a.name as author_name
        FROM books b
        JOIN authors a ON b.author_id = a.id
        ORDER BY b.title, a.name, b.publication_year
        )"
    );

    std::vector<std::pair<domain::Book, std::string>> books;
    for (size_t i = 0; i < result.size(); ++i) {
        const auto& row = result[i];

        domain::BookId book_id = domain::BookId::FromString(row["id"].as<std::string>());

        auto tags_result = work.exec_params(
            R"(
            SELECT tag FROM book_tags WHERE book_id = $1 ORDER BY tag
            )", book_id.ToString()
        );

        std::vector<std::string> tags;
        for (size_t i = 0; i < tags_result.size(); ++i) {
            tags.push_back(tags_result[i]["tag"].as<std::string>());
        }

        books.emplace_back(
            domain::Book{
                book_id,
                domain::AuthorId::FromString(row["author_id"].as<std::string>()),
                row["title"].as<std::string>(),
                row["publication_year"].as<int>(),
                std::move(tags)
            },
            row["author_name"].as<std::string>()
        );
    }


    return books;
}

void BookRepositoryImpl::Edit(const domain::BookId& book_id, const std::optional<std::string>& new_title,
    const std::optional<int>& new_pub_year, const std::vector<std::string>& new_tags) {
    pqxx::work work{ connection_, "serializable" };

    try {
        if (new_title.has_value() && !new_title.value().empty()) {
            work.exec_params(
                "UPDATE books SET title = $2 WHERE id = $1",
                book_id.ToString(), new_title.value()
            );
        }

        if (new_pub_year.has_value()) {
            work.exec_params(
                "UPDATE books SET publication_year = $2 WHERE id = $1",
                book_id.ToString(), new_pub_year.value()
            );
        }

        work.exec_params(
            "DELETE FROM book_tags WHERE book_id = $1",
            book_id.ToString()
        );

        if (!new_tags.empty()) {
            for (const auto& tag : new_tags) {
                work.exec_params(
                    "INSERT INTO book_tags (book_id, tag) VALUES ($1, $2)",
                    book_id.ToString(), tag
                );
            }
        }

        work.commit();
    }
    catch (const std::exception& e) {
        work.abort();
        throw std::runtime_error("Failed to edit book");
    }
}

void BookRepositoryImpl::Delete(const domain::BookId& book_id) {
    pqxx::work work{ connection_ , "serializable" };

    try {
        work.exec_params(
            "DELETE FROM book_tags WHERE book_id = $1",
            book_id.ToString()
        );

        work.exec_params(
            "DELETE FROM books WHERE id = $1",
            book_id.ToString()
        );

        work.commit();
    }
    catch (const std::exception& e) {
        work.abort();
        throw std::runtime_error("Failed to delete book");
    }
}

std::vector<domain::Book> BookRepositoryImpl::GetBooksByAuthorId(const domain::AuthorId& author_id) {
    pqxx::work work{ connection_ , "serializable" };
    auto result = work.exec_params("SELECT id, title, author_id, publication_year FROM books WHERE author_id = $1 ORDER BY publication_year, title",
        author_id.ToString());

    std::vector<domain::Book> books;
    for (size_t i = 0; i < result.size(); ++i) {
        const auto& row = result[i];
        books.push_back({
            domain::BookId::FromString(row["id"].as<std::string>()),
            domain::AuthorId::FromString(row["author_id"].as<std::string>()),
            row["title"].as<std::string>(),
            row["publication_year"].as<int>(), {}
            });
    }

    return books;
}

Database::Database(pqxx::connection connection)
    : connection_{ std::move(connection) } {
    pqxx::work work{ connection_ };
    work.exec(R"(
CREATE TABLE IF NOT EXISTS authors (
    id UUID PRIMARY KEY,
    name VARCHAR(100) UNIQUE NOT NULL
);
)"_zv);
    work.exec(R"(
CREATE TABLE IF NOT EXISTS books (
    id UUID PRIMARY KEY,
    author_id UUID NOT NULL,
    title VARCHAR(100) NOT NULL,
    publication_year INT NOT NULL,
    FOREIGN KEY (author_id) REFERENCES authors (id)
);
)"_zv);
    work.exec(R"(
CREATE TABLE IF NOT EXISTS book_tags (
    book_id UUID NOT NULL,
    tag VARCHAR(30) NOT NULL,
    FOREIGN KEY (book_id) REFERENCES books (id)
);
)"_zv);
    work.commit();
}

std::vector<domain::Author> AuthorRepositoryImpl::GetAuthors() {
    pqxx::work work{ connection_ , "serializable" };
    auto result = work.exec("SELECT id, name FROM authors ORDER BY name");

    std::vector<domain::Author> tmpRes;
    for (size_t i = 0; i < result.size(); ++i) {
        const auto& row = result[i];
        tmpRes.push_back({
            domain::AuthorId::FromString(row["id"].as<std::string>()),
            row["name"].as<std::string>()
            });
    }

    return tmpRes;
}


}  // namespace postgres