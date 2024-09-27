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

void UseCasesImpl::DeleteTag(const std::string& name) {
    tags_.Delete( name );
}

void UseCasesImpl::DeleteAuthor(const std::string& author_id, const std::string& name) {
    authors_.Delete({ AuthorId::FromString(author_id), name});
}

domain::Author UseCasesImpl::GetAuthorsByAuthorId(const std::string& author_id) {
    return authors_.GetAuthorById(AuthorId::FromString(author_id));
}

std::set<std::string> UseCasesImpl::GetTagsByBookId(domain::BookId book_id) {
    return tags_.GetTagsByBookId(book_id);
}

void UseCasesImpl::DeleteBook(const domain::Book& book) {
    books_.Delete(book);
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

}  // namespace app
