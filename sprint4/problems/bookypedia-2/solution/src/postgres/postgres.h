#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include <set>

#include "author.h"
#include "book.h"
#include "tags.h"

namespace postgres {

class AuthorRepositoryImpl : public domain::AuthorRepository {
public:
    explicit AuthorRepositoryImpl(pqxx::connection& connection)
        : connection_{connection} {
    }

    void Save(const domain::Author& author) override;
    void Delete(pqxx::work& work, const domain::Author& author) override;
    void Edit(const domain::Author& author, const std::string& new_name) override;
    void Commit() override;
    std::vector<domain::Author> GetAuthors() override;
    domain::Author GetAuthorById(const domain::AuthorId& id) override;
private:
    pqxx::connection& connection_;
};

class BookRepositoryImpl : public domain::BookRepository {
public:
    explicit BookRepositoryImpl(pqxx::connection& connection)
        : connection_(connection) {
    }

    void Save(const domain::Book& book) override;
    void Delete(pqxx::work& work,const domain::Book& book) override;
    void Edit(const domain::Book& book, const std::string& new_name) override;
    void EditYear(const domain::BookId& id, const std::string& new_year) override;
    std::vector<domain::Book> GetBooks() override;
    std::vector<domain::Book> GetBooksByAuthorId(const domain::AuthorId& author_id) override;
    domain::Book GetBookById(const domain::BookId & id) override;

private:
    pqxx::connection& connection_;
};

class TagRepositoryImpl : public domain::TagRepository {
public:
    explicit TagRepositoryImpl(pqxx::connection& connection)
        : connection_(connection) {
    }

    void Save(const domain::Tag& tag) override;
    void Delete(pqxx::work& work,const std::string& tag) override;
    void DeleteAllTagsByBook(const domain::Book book) override;
    std::set<std::string> GetTagsByBookId( const domain::BookId& book_id) override;
    std::set<std::string> GetTagsByBookIdInped(pqxx::work& work, const domain::BookId& book_id) override;
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

    TagRepositoryImpl& GetTags()& {
        return tags_;
    }

    pqxx::connection& GetConnect() {
        return connection_;
    }

private:
    pqxx::connection connection_;
    AuthorRepositoryImpl authors_{ connection_ };
    BookRepositoryImpl books_{ connection_ };
    TagRepositoryImpl tags_{ connection_ };
};

}  // namespace postgres