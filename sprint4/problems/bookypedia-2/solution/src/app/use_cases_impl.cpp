#include "use_cases_impl.h"
#include "author.h"
#include "book.h"

#include <ranges>
#include <algorithm>
#include <sstream>

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string& name) {
    authors_.Save({AuthorId::New(), name});
}

std::vector<domain::Author> UseCasesImpl::GetAuthors() {
    return authors_.GetAuthors();
}

void UseCasesImpl::AddBook(const std::string& author_id, const std::string& title, int publication_year) {
    books_.Save({ BookId::New(), AuthorId::FromString(author_id), title, publication_year });
}

std::vector<domain::Book> UseCasesImpl::GetBooks() {
    return books_.GetBooks();
}

std::vector<domain::Book> UseCasesImpl::GetBooksByAuthorId(const std::string& author_id) {
    return books_.GetBooksByAuthorId(AuthorId::FromString(author_id));
}

void UseCasesImpl::AddTags(const domain::BookId& id, const std::string& tag) {
    tags_.Save({ TagId::New(), id , tag });
}

void UseCasesImpl::DeleteTag(pqxx::work& work,const std::string& name) {
    tags_.Delete(work, name );
}

domain::Author UseCasesImpl::GetAuthorsByAuthorId(const std::string& author_id) {
    return authors_.GetAuthorById(AuthorId::FromString(author_id));
}

std::set<std::string> UseCasesImpl::GetTagsByBookId(domain::BookId book_id) {
    return tags_.GetTagsByBookId(book_id);
}

void UseCasesImpl::DeleteBook(const domain::Book& book) {
    pqxx::work work{ connection_ };
    DeleteBookForAuthor(work, book);
    work.commit();
}

void UseCasesImpl::EditAuthor(const domain::Author& author, const std::string& new_name) {
    authors_.Edit(author, new_name);
}

void UseCasesImpl::EditBook(const domain::Book& book, const std::string& new_name) {
    books_.Edit(book, new_name);
}

void UseCasesImpl::EditBookYear(const domain::BookId& id, const std::string& new_year) {
    books_.EditYear(id, new_year);
}

void UseCasesImpl::DeleteAllTagsByBook(const domain::Book& book) {
    tags_.DeleteAllTagsByBook(book);
}

void UseCasesImpl::DeleteAuthor(const std::string& id, const std::string& name) {

    auto books = books_.GetBooksByAuthorId(domain::AuthorId::FromString(id));
    auto author = authors_.GetAuthorById(domain::AuthorId::FromString(id));

    pqxx::work work{ connection_ };
    for (const auto& book : books)
    {
        DeleteBookForAuthor(work, book);

    }
    authors_.Delete(work, author);
    work.commit();
}

void UseCasesImpl::DeleteBookForAuthor(pqxx::work& work, const domain::Book& book) {

    auto tags = tags_.GetTagsByBookIdInped(work, book.GetBookId());
    for (const auto& tag : tags) {
        DeleteTag(work ,tag);
    }
    books_.Delete(work, book);
}

}  // namespace app
