#pragma once

#include "book.h"

#include <optional>
#include <set>
#include <string>
#include <vector>

namespace app {

class UseCases {
public:
    /*Сеттеры*/
    virtual void AddAuthor(const std::string& name) = 0;
    virtual void AddBook(const std::string& author_id, const std::string& title, int publication_year, std::vector<std::string> tags) = 0;

    /*Геттеры*/
    virtual std::vector<domain::Author> GetAuthors() = 0;
    virtual std::vector<std::pair<domain::Book, std::string>> GetBooks() = 0;
    virtual std::vector<domain::Book> GetBooksByAuthorId(const std::string& author_id) = 0;

    /*Эдиторы*/
    virtual void EditAuthor(const std::string& author_id, const std::string& new_name) = 0;
    virtual void EditBook(const std::string& book_id, const std::optional<std::string>& new_title,
        const std::optional<int>& new_pub_year, const std::vector<std::string>& new_tags) = 0;

    /*Делетеры*/
    virtual void DeleteAuthor(const std::string& author_id) = 0;
    virtual void DeleteBook(const std::string& book_id) = 0;

protected:
    ~UseCases() = default;
};

}  // namespace app
