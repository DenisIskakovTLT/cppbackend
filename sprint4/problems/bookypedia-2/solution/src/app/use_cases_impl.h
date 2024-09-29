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
    void AddBook(const std::string& author_id, const std::string& title, int publication_year, std::vector<std::string> tags) override;

    /*Геттеры*/
    std::vector<domain::Author> GetAuthors() override;
    std::vector<std::pair<domain::Book, std::string>> GetBooks() override;
    std::vector<domain::Book> GetBooksByAuthorId(const std::string& author_id) override;

    /*Эдиторы*/
    void EditAuthor(const std::string& author_id, const std::string& new_name) override;
    void EditBook(const std::string& book_id, const std::optional<std::string>& new_title,
        const std::optional<int>& new_pub_year, const std::vector<std::string>& new_tags) override;

    /*Делетеры*/
    void DeleteAuthor(const std::string& author_id) override;
    void DeleteBook(const std::string& book_id) override;

private:
    domain::AuthorRepository& authors_;
    domain::BookRepository& books_;
};

}  // namespace app
