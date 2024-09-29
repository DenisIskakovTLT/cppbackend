#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include "author.h"
#include "book.h"
#include "tagged_uuid.h"

namespace postgres {

    class AuthorRepositoryImpl : public domain::AuthorRepository {
    public:
        explicit AuthorRepositoryImpl(pqxx::connection& connection)
            : connection_{ connection } {
        }

        void Save(const domain::Author& author) override;

        void Delete(const domain::AuthorId& author_id) override;

        std::vector<domain::Author> GetAuthors() override;

        void Edit(const domain::AuthorId& author_id, const std::string& new_name) override;
    private:
        pqxx::connection& connection_;
    };

    class BookRepositoryImpl : public domain::BookRepository {
    public:
        explicit BookRepositoryImpl(pqxx::connection& connection)
            : connection_{ connection } {
        }

        void Save(const domain::Book& book) override;

        void Delete(const domain::BookId& book_id) override;

        std::vector<std::pair<domain::Book, std::string>> GetBooks() override;

        std::vector<domain::Book> GetBooksByAuthorId(const domain::AuthorId& author_id) override;

        void Edit(const domain::BookId& book_id, const std::optional<std::string>& new_title,
            const std::optional<int>& new_pub_year, const std::vector<std::string>& new_tags) override;
    private:
        pqxx::connection& connection_;
    };

class Database {
public:
    explicit Database(pqxx::connection connection);

    AuthorRepositoryImpl& GetAuthors() & {
        return authors_;
    }

    BookRepositoryImpl& GetBooks()& {
        return books_;
    }

private:
    pqxx::connection connection_;
    AuthorRepositoryImpl authors_{connection_};
    BookRepositoryImpl books_{ connection_ };
};

}  // namespace postgres