#pragma once
#include <pqxx/pqxx>
#include "author.h"
#include "book.h"
#include "tags.h"

#include <optional>
#include <string>
#include <set>
#include <vector>

namespace app {

class UseCases {
public:
    /*Сеттеры*/
    virtual void AddAuthor(const std::string& name) = 0;
    virtual void AddBook(const std::string& author_id, const std::string& title, int publication_year) = 0;
    virtual void AddTags(const domain::BookId& id, const std::string& tag) = 0;

    /*Геттеры*/
    virtual std::vector<domain::Author> GetAuthors() = 0;
    virtual std::vector<domain::Book> GetBooks() = 0;
    virtual std::vector<domain::Book> GetBooksByAuthorId(const std::string& author_id) = 0;
    virtual domain::Author GetAuthorsByAuthorId(const std::string& author_id) = 0;
    virtual std::set<std::string> GetTagsByBookId(domain::BookId book_id) = 0;

    /*Делетеры*/
    virtual void DeleteAuthor(const std::string& id, const std::string& name) = 0;
    virtual void DeleteTag(pqxx::work& work,const std::string& name) = 0;
    virtual void DeleteBook(const domain::Book& book) = 0;
    virtual void DeleteBookForAuthor(pqxx::work& work, const domain::Book& book) = 0;
    virtual void DeleteAllTagsByBook(const domain::Book& book) = 0;

    /*Эдиторы*/
    virtual void EditAuthor(const domain::Author& author, const std::string& new_name) = 0;
    virtual void EditBook(const domain::Book& book, const std::string& new_name) = 0;
    virtual void EditBookYear(const domain::BookId& id, const std::string& new_year) = 0;

protected:
    ~UseCases() = default;
};

}  // namespace app
