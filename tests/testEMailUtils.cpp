//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
//

#include "liblobby/EMailUtils.h"
#include <boost/test/unit_test.hpp>

using namespace s25lobby;

BOOST_AUTO_TEST_CASE(InvalidEMail)
{
    BOOST_TEST(getAnonymizedEmail("") == "<invalid email>");
    BOOST_TEST(getAnonymizedEmail("foobar.com") == "<invalid email>");
    BOOST_TEST(getAnonymizedEmail("@foobar.com") == "<invalid email>");
    BOOST_TEST(getAnonymizedEmail("foo@") == "<invalid email>");
}

BOOST_AUTO_TEST_CASE(ValidEMail)
{
    BOOST_TEST(getAnonymizedEmail("foo@bar.com") == "f*o@b*****m");
    // Each letter is replaced
    BOOST_TEST(getAnonymizedEmail("f12345o@b1234m") == "f*****o@b****m");
    // Single letter before/after @ are handled
    BOOST_TEST(getAnonymizedEmail("f@bar.com") == "f@b*****m");
    BOOST_TEST(getAnonymizedEmail("foo@b") == "f*o@b");
    BOOST_TEST(getAnonymizedEmail("f@b") == "f@b");
}
