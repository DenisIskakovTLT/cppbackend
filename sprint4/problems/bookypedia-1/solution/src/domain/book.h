#pragma once
#include "author.h"
#include "tagged_uuid.h"

#include <string>
#include <vector>

namespace domain {
	
	/*По аналогии с Authors*/
	namespace detail {
		struct BookTag {};
	}  // namespace detail

	using BookId = util::TaggedUUID<detail::BookTag>;

	class Book {
	public:
		Book(BookId book_id, AuthorId author_id, std::string title, int year)
			: id_(std::move(book_id))
			, author_id_(std::move(author_id))
			, title_(std::move(title))
			, publication_year_(year) {
		}

		const BookId& GetBookId() const noexcept;

		const AuthorId& GetAuthorId() const noexcept;

		const std::string& GetTitle() const noexcept;

		int GetPublicationYear() const noexcept;

	private:
		BookId id_;
		AuthorId author_id_;
		std::string title_;
		int publication_year_;
	};

	class BookRepository {
	public:
		virtual void Save(const Book& book) = 0;
		virtual Book GetBookById(const BookId& id) = 0;
		virtual std::vector<Book> GetBooks() = 0;
		virtual std::vector<Book> GetBooksByAuthorId(const domain::AuthorId& author_id) = 0;

	protected:
		~BookRepository() = default;
	};

}