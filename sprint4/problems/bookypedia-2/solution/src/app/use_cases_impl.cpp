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

void UseCasesImpl::AddBook(const std::string& author_id, const std::string& title, int publication_year, std::vector<std::string> tags) {
    books_.Save({ BookId::New(), domain::AuthorId::FromString(author_id), title, publication_year , tags });
}

std::vector<std::pair<domain::Book, std::string>> UseCasesImpl::GetBooks() {
    return books_.GetBooks();
}

std::vector<domain::Book> UseCasesImpl::GetBooksByAuthorId(const std::string& author_id) {
    return books_.GetBooksByAuthorId(AuthorId::FromString(author_id));
}

void UseCasesImpl::DeleteAuthor(const std::string& author_id) {
    authors_.Delete(domain::AuthorId::FromString(author_id));
}

void UseCasesImpl::EditAuthor(const std::string& author_id, const std::string& new_name) {
    authors_.Edit(domain::AuthorId::FromString(author_id), new_name);
}

void UseCasesImpl::DeleteBook(const std::string& book_id) {
    books_.Delete(domain::BookId::FromString(book_id));
}

void UseCasesImpl::EditBook(const std::string& book_id, const std::optional<std::string>& new_title,
    const std::optional<int>& new_pub_year, const std::vector<std::string>& new_tags) {
    books_.Edit(domain::BookId::FromString(book_id), new_title, new_pub_year, new_tags);
}

}  // namespace app
