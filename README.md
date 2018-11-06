[![Travis Status](https://travis-ci.org/dutow/tsar.svg?branch=master)](https://travis-ci.org/dutow/tsar)
[![AppVeyor Status](https://ci.appveyor.com/api/projects/status/github/dutow/tsar)](https://ci.appveyor.com/api/projects/status/github/dutow/tsar)

What's this?
===

`tsar` is a C++17 library providing two new (named and sorted) tuple types:

 * the `standard_tuple`, which is a guaranteed standard layout type, no matter the members, and knows the offsets of each data member
 * the `context_aware_tuple`, which allows the tuple members to access each other.

Motivation
---

The motivation behind these types is to provide an easy way to implement types such as:

 * **Memory efficient, user friendly observers**: observable types, where the list of observers is storend in the container, yet the `observe` function is added to each data member. An example implementation can be found in `tsar::observable`.
 * **Typesafe vertex attributes**: Graphical applications usually represent vertex data using untyped arrays, accessing members with pointers with needed. Standard tuples provide a simple way to define vertex layouts with an automatically generated data type.
 * **Pure C++ ORM framework**: A simple object relational mapping library, without compiler extensions is one of the greatest challanges in C++. Similarly to the observable types, it can also be implemented using context aware types, without depending on external tools.
 * **A user friendly, type safe way to implement modular applications**, where modules all have requirements (such as about the existence of other modules), and can be used as simple building blocks. This would allow several configurable embedded applications (printers, cars, and so on) to use the C++ type system instead of runtime or macro based configuration.

