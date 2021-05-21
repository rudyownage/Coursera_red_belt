#pragma once
template <typename It>
class Paginator {
public:
    Paginator(It b, It e, size_t size) {
        for (size_t left = distance(b, e); left > 0;) {
            size_t current_page_size = min(size, left);
            It current_page_end = next(b, current_page_size);
            pages.push_back({ b, current_page_end });
            left -= current_page_size;
            b = current_page_end;
        }
    }
    auto begin() const {
        return pages.begin();
    }
    auto end() const {
        return pages.end();
    }
    size_t size() const {
        return pages.size();
    }

private:
    vector<IteratorRange<It>> pages;
};

template <typename C>
auto Paginate(C& c, size_t page_size) {
    return Paginator(c.begin(), c.end(), page_size);
}