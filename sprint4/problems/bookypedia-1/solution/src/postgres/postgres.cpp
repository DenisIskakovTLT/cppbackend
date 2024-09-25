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

void BookRepositoryImpl::Save(const domain::Book& book) {
    pqxx::work work{ connection_ };
    /*https://habr.com/ru/articles/264281/ используем ON CONFLICT, при одинаковых айди заменит содержимое*/
    work.exec_params(
        R"(
INSERT INTO books (id, author_id, title, publication_year) VALUES ($1, $2, $3, $4)
ON CONFLICT (id) DO UPDATE SET author_id=$2, title=$3, publication_year=$4;
)"_zv,
book.GetBookId().ToString(), book.GetAuthorId().ToString(), book.GetTitle(), book.GetPublicationYear());
    work.commit();
}

Database::Database(pqxx::connection connection)
    : connection_{std::move(connection)} {
    pqxx::work work{connection_};
    work.exec(R"(
CREATE TABLE IF NOT EXISTS authors (
    id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
    name varchar(100) UNIQUE NOT NULL
);
)"_zv);
    // ... создать другие таблицы   
    /*REFERENCES связывает таблицы в данном случае authors https://metanit.com/sql/postgresql/2.5.php*/
    work.exec(R"(
CREATE TABLE IF NOT EXISTS books (
    id UUID CONSTRAINT book_id_constraint PRIMARY KEY,
    author_id UUID NOT NULL REFERENCES authors(id),                     
    title varchar(100) NOT NULL,
    publication_year INTEGER
);
)"_zv);
    // коммитим изменения
    work.commit();
}

domain::Author AuthorRepositoryImpl::GetAuthorById(const domain::AuthorId& id) {
    pqxx::read_transaction transact(connection_);
    auto q = R"(
            SELECT name FROM authors 
            WHERE id = )" + id.ToString() + R"( 
            LIMIT 1;)";

    auto [name] = transact.query1<std::string>(q);      //Используем query1, возвращает только одну строку

    return domain::Author{ id, name };
}

std::vector<domain::Author> AuthorRepositoryImpl::GetAuthors() {
    pqxx::read_transaction transact(connection_);
    auto q = R"(SELECT id, name FROM authors ORDER BY name;)";

    std::vector<domain::Author> tmpRes;

    for (auto [id, name] : transact.query<std::string, std::string>(q)) {
        tmpRes.emplace_back(domain::Author{ domain::AuthorId::FromString(id), name });
    }

    return tmpRes;
}

domain::Book BookRepositoryImpl::GetBookById(const domain::BookId& id) {
    pqxx::read_transaction transact(connection_);
    auto q = R"(
SELECT author_id, title, publication_year FROM books 
WHERE id = )" + transact.quote(id.ToString()) + R"( 
LIMIT 1;)";

    auto [author_id, title, year] = transact.query1<std::string, std::string, int>(q);

    return domain::Book{ id, domain::AuthorId::FromString(author_id), title, year };
}

std::vector<domain::Book> BookRepositoryImpl::GetBooks() {
    pqxx::read_transaction transact(connection_);
    auto q = R"(
SELECT id, author_id, title, publication_year FROM books 
ORDER BY title;)";

    std::vector<domain::Book> tmpRes;

    for (auto [id, author_id, title, year] : transact.query<std::string, std::string, std::string, int>(q)) {
        tmpRes.emplace_back(domain::Book{ domain::BookId::FromString(id), domain::AuthorId::FromString(author_id), title, year });
    }

    return tmpRes;
}

std::vector<domain::Book> BookRepositoryImpl::GetBooksByAuthorId(const domain::AuthorId& author_id) {
    pqxx::read_transaction transact(connection_);
    auto q = R"(
SELECT id, author_id, title, publication_year FROM books 
WHERE author_id = )" + transact.quote(author_id .ToString()) + R"( 
ORDER BY publication_year, title;)";

    std::vector<domain::Book> res;

    for (auto [id, author_id, title, year] : transact.query<std::string, std::string, std::string, int>(q)) {
        res.emplace_back(domain::Book{ domain::BookId::FromString(id), domain::AuthorId::FromString(author_id), title, year });
    }

    return res;
}

}  // namespace postgres