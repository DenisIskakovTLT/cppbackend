#pragma once
#include "../domain/author_fwd.h"
#include "use_cases.h"
#include "book.h"

#include <optional>

namespace app {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::AuthorRepository& authors, domain::BookRepository& books)
        : authors_{authors}, books_{ books } {
    }

    /*Сеттеры*/
    void AddAuthor(const std::string& name) override;
    void AddBook(const std::string& author_id, const std::string& title, int publication_year) override;

    /*Геттеры*/
    std::vector<domain::Author> GetAuthors() override;
    std::vector<domain::Book> GetBooks() override;
    std::vector<domain::Book> GetBooksByAuthorId(const std::string& author_id) override;

private:
    domain::AuthorRepository& authors_;
    domain::BookRepository& books_;
};

}  // namespace app
