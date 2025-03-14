#include <windows.h>
#include <vector>

struct field {
    RECT position; 
};

class board
{
public:
    static constexpr LONG margin = 5;
    static constexpr LONG field_size = 90;
    LONG size;
    using field_array = std::vector<field>;

    board(int gridSize) : N(gridSize) {
        initializeFields();
        size = N * (field_size + margin) + margin;
    }
    field_array const& fields() const { return m_fields; }

    int getFieldIndex(int x, int y) const {
        for (size_t i = 0; i < m_fields.size(); ++i) {
            if (PtInRect(&m_fields[i].position, { x, y })) {
                return i;
            }
        }
        return -1;
    }

private:
    int N;
    field_array m_fields;

    void initializeFields() {
        for (int row = 0; row < N; ++row) {
            for (int col = 0; col < N; ++col) {
                int left = col * (field_size + margin) + margin;
                int top = row * (field_size + margin) + margin;
                RECT rect = { left, top, left + field_size, top + field_size };
                m_fields.push_back({ rect });
            }
        }
    }
};
