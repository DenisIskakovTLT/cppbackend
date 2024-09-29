#include "book.h"

namespace domain {

	const BookId& Book::GetBookId() const noexcept {
		return id_;
	}

	const AuthorId& Book::GetAuthorId() const noexcept {
		return author_id_;
	}

	const std::string& Book::GetTitle() const noexcept {
		return title_;
	}

	int Book::GetPublicationYear() const noexcept {
		return publication_year_;
	}

	const std::vector<std::string>& Book::GetTags() const noexcept {
		return tags_;
	}
}