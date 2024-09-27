#include <catch2/catch_test_macros.hpp>

#include "../src/app/use_cases_impl.h"
#include "../src/domain/author.h"
#include "../src/domain/book.h"

namespace {

    struct MockAuthorRepository : domain::AuthorRepository {
        std::vector<domain::Author> saved_authors;

        void Save(const domain::Author& author) override {
            saved_authors.emplace_back(author);
        }

        domain::Author GetAuthorById(const domain::AuthorId& id) override {
            for (const auto& author : saved_authors) {
                if (author.GetId() == id) {
                    return author;
                }
            }
            throw std::runtime_error("Author not found");
        }

        std::vector<domain::Author> GetAuthors() override {
            return saved_authors;
        }
    };

    struct MockBookRepository : domain::BookRepository {
        std::vector<domain::Book> saved_books;

        void Save(const domain::Book& book) override {
            saved_books.emplace_back(book);
        }

        domain::Book GetBookById(const domain::BookId& id) override {
            for (const auto& book : saved_books) {
                if (book.GetBookId() == id) {
                    return book;
                }
            }
            throw std::runtime_error("Book not found");
        }

        std::vector<domain::Book> GetBooks() override {
            return saved_books;
        }

        std::vector<domain::Book> GetBooksByAuthorId(const domain::AuthorId& author_id) override {
            std::vector<domain::Book> result;
            for (const auto& book : saved_books) {
                if (book.GetAuthorId() == author_id) {
                    result.emplace_back(book);
                }
            }
            return result;
        }
    };

    struct Fixture {
        MockAuthorRepository authors;
        MockBookRepository books;
    };

}  // namespace

SCENARIO_METHOD(Fixture, "Book Adding") {
    GIVEN("Use cases") {
        app::UseCasesImpl use_cases{ authors, books };

        WHEN("Adding an author") {
            const auto author_name = "Joanne Rowling";
            use_cases.AddAuthor(author_name);

            THEN("author with the specified name is saved to repository") {
                REQUIRE(authors.saved_authors.size() == 1);
                CHECK(authors.saved_authors.at(0).GetName() == author_name);
                CHECK(authors.saved_authors.at(0).GetId() != domain::AuthorId{});
            }

            WHEN("Show authors") {
                //use_cases.ShowAuthors();

                THEN("author with the specified name is saved to repository") {
                    // Проверить, что в выводе команды присутсвует правильный автор
                }
            }
        }

    }
}