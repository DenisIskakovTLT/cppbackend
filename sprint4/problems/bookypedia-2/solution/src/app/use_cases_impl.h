#pragma once
#include "../domain/author_fwd.h"
#include "use_cases.h"
#include "book.h"
#include "tags.h"

#include <optional>

namespace app {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::AuthorRepository& authors, domain::BookRepository& books, domain::TagRepository& tags)
        : authors_{authors}, books_{ books }, tags_(tags) {
    }

    /*Сеттеры*/
    void AddAuthor(const std::string& name) override;
    void AddBook(const std::string& author_id, const std::string& title, int publication_year) override;
    void AddTags(const domain::BookId& id, const std::string& tag) override;

    /*Геттеры*/
    std::vector<domain::Author> GetAuthors() override;
    std::vector<domain::Book> GetBooks() override;
    std::vector<domain::Book> GetBooksByAuthorId(const std::string& author_id) override;
    domain::Author GetAuthorsByAuthorId(const std::string& author_id) override;
    std::set<std::string> GetTagsByBookId(domain::BookId book_id) override;

    /*Делетеры*/
    void DeleteAuthor(const std::string& author_id, const std::string& name) override;
    void DeleteTag(const std::string& name) override;
    void DeleteBook(const domain::Book& book) override;
    void DeleteAllTagsByBook(const domain::Book& book) override;

    /*Эдиторы*/
    void EditAuthor(const domain::Author& author, const std::string& new_name) override;
    void EditBook(const domain::Book& book, const std::string& new_name) override;
    void EditBookYear(const domain::BookId& id, const std::string& new_year) override;

private:
    domain::AuthorRepository& authors_;
    domain::BookRepository& books_;
    domain::TagRepository& tags_;
};

}  // namespace app
