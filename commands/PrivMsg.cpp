
// PRIVMSG <nick> :message
// PRIVMSG #channel :message

//     If target is a user → send directly

//     If target is a channel → broadcast

//     If no such nick → 401 ERR_NOSUCHNICK

//     If no text → 412 ERR_NOTEXTTOSEND