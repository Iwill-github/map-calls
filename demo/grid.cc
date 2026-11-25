namespace {

struct ColumnView {
    const unsigned* first;
    unsigned count;
    unsigned stride;
};

const unsigned* secondRow(const unsigned* values)
{
    return values + 2;
}

unsigned elementOffset(unsigned row, unsigned column, unsigned columns)
{
    return row * columns + column;
}

const unsigned* gridElement(const unsigned* values, unsigned row, unsigned column, unsigned columns)
{
    return values + elementOffset(row, column, columns);
}

ColumnView makeColumnView(const unsigned* values, unsigned rows, unsigned columns, unsigned column)
{
    return {gridElement(values, 0, column, columns), rows, columns};
}

unsigned readValue(const unsigned* values)
{
    return *values;
}

unsigned sumRange(const unsigned* values, unsigned count, unsigned stride)
{
    const unsigned current = readValue(values);
    if (count == 1) {
        return current;
    }
    return current + sumRange(values + stride, count - 1, stride);
}

unsigned sumRow(const unsigned* values)
{
    return sumRange(secondRow(values), 2, 1);
}

unsigned sumColumn(const unsigned* values)
{
    const ColumnView column = makeColumnView(values, 2, 2, 1);
    return sumRange(column.first, column.count, column.stride);
}

} // namespace

// The input is a 2-by-2 grid stored in row-major order.
unsigned sumGrid(const unsigned* values, bool byRow)
{
    if (byRow) {
        return sumRow(values);
    }
    return sumColumn(values);
}
