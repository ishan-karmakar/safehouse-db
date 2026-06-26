pub const meta = @import("meta.zig");
pub const Statement = @import("Statement.zig");
pub const Row = @import("Row.zig");
pub const Table = @import("Table.zig");

const std = @import("std");
const log = std.log;

pub fn main(init: std.process.Init) !u8 {
    var stdout_buf: [1024]u8 = undefined;
    var stdin_buf: [1024]u8 = undefined;

    var stdout = std.Io.File.stdout().writer(init.io, &stdout_buf);
    var stdin = std.Io.File.stdin().reader(init.io, &stdin_buf);

    const table = try std.heap.smp_allocator.create(Table);
    table.* = .{};

    while (true) {
        try stdout.interface.print("> ", .{});
        try stdout.interface.flush();

        const prompt = try stdin.interface.takeDelimiter('\n') orelse continue;

        if (prompt[0] == '.') {
            meta.handle_command(prompt) catch |err| switch (err) {
                error.UnrecognizedCommand => log.warn("Unrecognized command '{s}'", .{prompt}),
            };
            continue;
        }

        const statement = Statement.from(prompt) catch |err| {
            switch (err) {
                error.UnrecognizedStatement => log.warn("Unrecognized keyword at start of '{s}'.", .{prompt}),
                error.SyntaxError => log.warn("Syntax error. Could not parse statement.", .{}),
            }
            continue;
        };

        statement.execute(table) catch |err| {
            switch (err) {
                error.TableFull => log.err("Table full", .{}),
            }
            continue;
        };
        log.info("Executed", .{});
    }
}
