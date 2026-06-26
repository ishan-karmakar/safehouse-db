const root = @import("root");
const Row = root.Row;
const Table = root.Table;

const std = @import("std");
const log = std.log.scoped(.stmt);

pub const Type = enum {
    INSERT,
    SELECT,
};

type: Type,
row: Row,

pub fn from(prompt: []const u8) !@This() {
    if (std.mem.startsWith(u8, prompt, "insert")) {
        var it = std.mem.tokenizeScalar(u8, prompt, ' ');
        _ = it.next();
        return .{
            .type = .INSERT,
            .row = .{
                .id = std.fmt.parseInt(u32, it.next() orelse return error.SyntaxError, 10) catch return error.SyntaxError,
                .username = it.next() orelse return error.SyntaxError,
                .email = it.next() orelse return error.SyntaxError,
            },
        };
    } else if (std.mem.eql(u8, prompt, "select")) {
        return .{ .type = .SELECT, .row = undefined };
    }

    return error.UnrecognizedStatement;
}

pub fn execute(self: *const @This(), table: *Table) !void {
    try switch (self.type) {
        .INSERT => self.execute_insert(table),
        .SELECT => execute_select(table),
    };
}

fn execute_insert(self: *const @This(), table: *Table) !void {
    if (table.num_rows >= Table.MAX_ROWS)
        return error.TableFull;

    self.row.serialize(table.row_slot(table.num_rows));
    table.num_rows += 1;
}

fn execute_select(table: *Table) !void {
    for (0..table.num_rows) |i| {
        const row = Row.from(table.row_slot(@intCast(i)));
        row.print();
    }
}
