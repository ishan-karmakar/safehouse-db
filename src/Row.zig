id: u32,
username: []const u8,
email: []const u8,

pub fn from(source: []const u8) @This() {
    var row: @This() = undefined;
    @memcpy(&row.id, source[@offsetOf(@This(), "id")..]);
    @memcpy(&row.username, source[@offsetOf(@This(), "username")]);
    @memcpy(&row.email, source[@offsetOf(@This(), "email")]);
    return row;
}

pub fn serialize(self: *const @This(), dest: []u8) void {
    @memcpy(dest[@offsetOf(@This(), "id")..], &self.id);
    @memcpy(dest[@offsetOf(@This(), "username")], &self.username);
    @memcpy(dest[@offsetOf(@This(), "email")], &self.email);
}
