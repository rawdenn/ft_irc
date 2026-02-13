// What it does

// View or set the channel topic.

// Required checks

// Channel exists → 403 ERR_NOSUCHCHANNEL

// User must be in the channel → 442 ERR_NOTONCHANNEL

// If channel +t (topic restricted) → only operators can set topic

// Replies

// 331 RPL_NOTOPIC if no topic

// 332 RPL_TOPIC if topic exists