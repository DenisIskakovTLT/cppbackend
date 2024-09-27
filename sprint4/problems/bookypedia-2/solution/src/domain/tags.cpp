#include "tags.h"

namespace domain {

	const BookId& Tag::GetBookId() const noexcept {
		return id_;
	}

	const std::string& Tag::GetTag() const noexcept {
		return tags_;
	}

	const TagId& Tag::GetTagId() const noexcept {
		return tagId_;
	}

}