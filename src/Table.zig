const Row = @import("root").Row;

const std = @import("std");

const MAX_PAGES = 100;
const PAGE_SIZE = 0x1000;
const ROWS_PER_PAGE = PAGE_SIZE / @sizeOf(Row);
pub const MAX_ROWS = ROWS_PER_PAGE * MAX_PAGES;

num_rows: u32 = 0,
pages: [MAX_PAGES]?[]u8 = @splat(null),

pub fn deinit(self: *@This()) void {
    for (self.pages) |page| if (page) |p|
        std.heap.page_allocator.free(p);
}

pub fn row_slot(self: *@This(), row_num: u32) [*]u8 {
    const page_num = row_num / ROWS_PER_PAGE;
    var page = self.pages[page_num];
    if (page == null) {
        page = std.heap.page_allocator.alloc(u8, PAGE_SIZE) catch null;
        self.pages[page_num] = page;
    }
    const row_offset = row_num % ROWS_PER_PAGE;
    const byte_offset = row_offset * @sizeOf(Row);
    return page.?.ptr + byte_offset;
}
