const std = @import("std");
const log = std.log.scoped(.row);

id: u32,
username: []const u8,
email: []const u8,

pub fn from(source: [*]const u8) @This() {
    var row: @This() = undefined;
    @memcpy(std.mem.asBytes(&row.id), source[@offsetOf(@This(), "id")..]);
    @memcpy(std.mem.asBytes(&row.username), source[@offsetOf(@This(), "username")..]);
    @memcpy(std.mem.asBytes(&row.email), source[@offsetOf(@This(), "email")..]);
    return row;
}

pub fn serialize(self: *const @This(), dest: [*]u8) void {
    @memcpy(dest[@offsetOf(@This(), "id")..], std.mem.asBytes(&self.id));
    @memcpy(dest[@offsetOf(@This(), "username")..], std.mem.asBytes(&self.username));
    @memcpy(dest[@offsetOf(@This(), "email")..], std.mem.asBytes(&self.email));
}

pub fn print(self: *const @This()) void {
    log.info("({}, {s}, {s})", .{ self.id, self.username, self.email });
}
