const std = @import("std");

pub fn handle_command(prompt: []const u8) !void {
    if (std.mem.eql(u8, prompt, ".exit")) {
        std.process.exit(0);
    } else return error.UnrecognizedCommand;
}
