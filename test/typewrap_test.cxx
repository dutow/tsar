
#include "tsar/typewrap_literals.hpp"

#include <iostream>

using namespace tsar::literals;

static_assert((0_c).val() == 0);
static_assert((5_c).val() == 5);
static_assert((12_c).val() == 12);
static_assert((124513_c).val() == 124513);

int main() { return 0; }