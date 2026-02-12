Format
KICK #channel nickname :optional reason

What it does

Removes a user from a single channel.

Broadcasts the KICK message to all channel members.

Required checks

Channel exists → else 403 ERR_NOSUCHCHANNEL

You are operator → else 482 ERR_CHANOPRIVSNEEDED

Target user exists in channel → else 441 ERR_USERNOTINCHANNEL
Notes

The kicked user stays connected to the server.

If the channel becomes empty, it can be deleted.