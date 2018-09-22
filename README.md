[![Travis Status](https://travis-ci.org/dutow/tsar.svg?branch=master)](https://travis-ci.org/dutow/tsar)
[![AppVeyor Status](https://ci.appveyor.com/api/projects/status/github/dutow/tsar)](https://ci.appveyor.com/api/projects/status/github/dutow/tsar)

What's this?
===

`tsar` is a prototype for:

 * providing instantation context aware types for C++
 * with the ability to add requirements on the context with user friendly messages (this is a TODO)

As an example, see the following simple snippet:

```
struct throttle;
struct engine;
struct wheel;

struct car {
  throttle throttle_;
  engine engine_;
  wheel wheels_[4];
};

car car_;
std::cout << car_.wheels_[0].angular_velocity_;
car_.throttle_.press();
// should be higher now
std::cout << car_.wheels_[0].angular_velocity_;
```

The goal is to make this program work - with some changes, of course --, but without:

 * adding pointers to the other data members to the component classes
 * moving the press method to a different class
 * restricting the usability of the component types to this specific `car` type


