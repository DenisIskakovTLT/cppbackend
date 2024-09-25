#pragma once

#include "author.h"
#include "book.h"

#include <optional>
#include <string>
#include <vector>

namespace app {

class UseCases {
public:
    /*Сеттеры*/
    virtual void AddAuthor(const std::string& name) = 0;
    virtual void AddBook(const std::string& author_id, const std::string& title, int publication_year) = 0;

    /*Геттеры*/
    virtual std::vector<domain::Author> GetAuthors() = 0;
    virtual std::vector<domain::Book> GetBooks() = 0;
    virtual std::vector<domain::Book> GetBooksByAuthorId(const std::string& author_id) = 0;

protected:
    ~UseCases() = default;
};

}  // namespace app
