resources:

https://datatracker.ietf.org/doc/html/rfc1459#section-9.2.2
https://dd.ircdocs.horse/refs/commands/user


Command notes:

Invite:
adds invited user to invited user list. then invited user can join the channel. invited user is then removed from the invited user list

Mode:
i: Set/remove Invite-only channel

t: Set/remove the restrictions of the TOPIC command to channel operators

k: Set/remove the channel key (password) : TESTING

o: Give/take channel operator privilege : TESTING

l: Set/remove the user limit to channel:
if for example 10 users are in a channel, then user limit is set to 5. the limit only applies for future users trying to join. if a user leaves channel and tries to reenter, they cannot unless the number of users becomes under the limit