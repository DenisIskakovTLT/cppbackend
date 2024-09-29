#pragma once
#include "author.h"
#include "tagged_uuid.h"
#include "book.h"

namespace domain {

	/*По аналогии с Authors*/
	namespace detail {
		struct TagTag {};
	}  // namespace detail
	using TagId = util::TaggedUUID<detail::TagTag>;

	class Tag {
	public:

		Tag(TagId tagId, BookId book_id, std::string tag):
			tagId_(tagId),
			id_(book_id),
			tags_(tag)
		{

		}
		const TagId& GetTagId() const noexcept;
		const BookId& GetBookId() const noexcept;

		const std::string& GetTag() const noexcept;

	private:

		TagId tagId_;
		BookId id_;
		std::string tags_;
	};

	class TagRepository {
	public:
		virtual void Save(const domain::Tag& tag) = 0;
		virtual void Delete(pqxx::work& work, const std::string& name) = 0;
		virtual void DeleteAllTagsByBook(const domain::Book book) = 0;
		virtual std::set<std::string> GetTagsByBookId(const BookId& book_id) = 0;
		virtual std::set<std::string> GetTagsByBookIdInped(pqxx::work& work, const domain::BookId& book_id) = 0;
	protected:
		~TagRepository() = default;
	};

}