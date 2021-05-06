//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once

#include <string>

namespace s25lobby {

inline std::string getAnonymizedEmail(const std::string& email)
{
    const auto atPos = email.find('@');
    if(atPos == std::string::npos || atPos == 0u || atPos == email.size() - 1u)
        return "<invalid email>";
    std::string result = email.substr(0, 1);
    if(atPos >= 2u)
        result += std::string(atPos - 2u, '*') + email[atPos - 1u];
    result += email.substr(atPos, 2);
    if(atPos + 3u < email.size())
        result += std::string(email.size() - atPos - 3u, '*') + email.back();
    return result;
}

} // namespace s25lobby
