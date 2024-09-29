#pragma once
#include <iosfwd>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace menu {
class Menu;
}

namespace app {
class UseCases;
}

namespace ui {
namespace detail {

struct AddBookParams {
    std::string title;
    std::string author_id;
    int publication_year = 0;
};

struct AuthorInfo {
    std::string id;
    std::string name;
};

struct BookInfo {
    std::string title;
    int publication_year;
    std::string author_name;
    std::string book_id;
};

}  // namespace detail

class View {
public:
    View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output);

private:
    bool AddAuthor(std::istream& cmd_input) const;
    bool AddBook(std::istream& cmd_input) const;
    bool ShowAuthors() const;
    bool ShowAuthorsWithId() const;
    bool ShowBooks() const;
    bool ShowBook(std::istream& cmd_input) const;
    bool ShowAuthorBooks() const;
    bool DeleteAutor(std::istream& cmd_input) const;
    bool DeleteBook(std::istream& cmd_input) const;
    bool EditAuthor(std::istream& cmd_input) const;
    bool EditBook(std::istream& cmd_input) const;

    std::optional<detail::AddBookParams> GetBookParams(std::istream& cmd_input) const;
    std::optional<std::string> SelectAuthor() const;
    std::vector<detail::AuthorInfo> GetAuthors() const;
    std::vector<detail::BookInfo> GetBooks() const;
    std::vector<detail::BookInfo> GetAuthorBooks(const std::string& author_id) const;
    std::string GetNameByAuthorId(const std::string& author_id) const;

    ui::detail::AuthorInfo GetAuthorByName(const std::string& authorName) const;
    std::vector<ui::detail::BookInfo>  GetBookByName(const std::string& bookName) const;
    void AddBookTags(std::string book_name) const;
    std::set<std::string> SplitStringByChar(std::string string, char ch) const;
    void PrintAllAboutABook(ui::detail::BookInfo book) const;

    menu::Menu& menu_;
    app::UseCases& use_cases_;
    std::istream& input_;
    std::ostream& output_;
};

}  // namespace ui