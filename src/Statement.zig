const Row = @import("root").Row;

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

pub fn execute(statement: *const @This()) void {
    switch (statement.type) {
        .INSERT => log.info("This is where we would do an insert", .{}),
        .SELECT => log.info("This is where we would do a select", .{}),
    }
}
