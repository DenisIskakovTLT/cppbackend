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

}  // namespace app
