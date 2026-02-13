// Format
// INVITE nickname #channel

// What it does

// Invites a user to a channel, especially useful if the channel is +i (invite-only).

// Required checks

// Channel exists → 403 ERR_NOSUCHCHANNEL if not

// Operator privilege → 482 ERR_CHANOPRIVSNEEDED

// Target user exists

// Target user not already in channel
// Notes

// When invited user tries to JOIN an +i channel, allow them and remove from invited list.