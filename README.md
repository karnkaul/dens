# dens

[![Build status](https://ci.appveyor.com/api/projects/status/v6c113r44hw4ndry?svg=true)](https://ci.appveyor.com/project/karnkaul/dens)

A lightweight and simple archetype-based entity-component framework. `dens` is simply short for "dense" or "densely packed components".

## Features

- Header-only interface
- Based on archetypes, aka unique sets of component types
- No type / component registrations required
- Exclusion typelist for queries
- Multiple simultaneous registries
- Components stored directly as (type-erased) `std::vector<T>`
- Minimal type erasure: only one `void*` and `reinterpret_cast` throughout library

### Limitations

- Supports a maximum of one component instance per type per entity

## Usage

### Example

```cpp
#include <iostream>
#include <string>
#include <dens/registry.hpp>

int main() {
  dens::registry r;
  auto e1 = r.make_entity(); // no components attached
  auto e2 = r.make_entity<int, float>(); // default construct and attach int and float

  r.attach<int>(e1) = 42; // attach trivial
  r.attach<std::string>(e1, "hello"); // attach non-trivial

  r.get<int>(e2) = -9; // get reference to int (must be attached)
  if (auto f = r.find<float>(e2)) { *f = 3.14f; } // get pointer to float if attached

  // iterate over all ints and floats
  for (auto e : r.view<int, float>()) {
    auto [i, f] = e.components;
    std::cout << r.name(e) << ": int: " << i << "float: " << f << "\n";
  }
  // iterate over all strings that do not have floats or chars attached
  for (auto e : r.view<std::string>(dens::exclude<float, char>())) {
    std::cout << r.name(e) << ": int: " << e.get<int>() << "\n";
  }

  std::cout << "string detached from e1: " << r.detach<std::string>(e1) << "\n";
  std::cout << "entity count: " << r.size() << "\n";

  r.clear();
}
```

### Requirements

- CMake
- C++20 compiler (and stdlib)

### Integration

1. Clone repo / add a git submodule in an appropriate subdirectory, say `dens`
1. Add library to project via: `add_subdirectory(dens)` and `target_link_libraries(foo dens::dens)`
1. Use via `#include <dens/registry.hpp>`
1. Configure with `DENS_BUILD_TESTS=ON` to build tests executables in `tests`

### Architecture

The main motivations behind / goals for `dens` are for it to:

- Be lightweight and simple
- Focus on ergonomics without compromising too much on performance
- Use minimal type erasure / weak typing

For a detailed background on ECS, Michele Caini's [excellent articles on it](https://skypjack.github.io/), particularly the one [describing archetypes](https://skypjack.github.io/2019-03-07-ecs-baf-part-2/) are recommended. As a brief recap: an **archetype** represents a specific collection of component types, and owns all entities that have that exact set of components attached. Adding/removing components to an entity results in it being moved between corresponding archetypes, and component queries returns a list of entities whose archetypes have at least the desired types attached.

```
archetype => components types
A0 => P, Q
A1 => P, S
A2 => P, Q, R

query<P> => [A0, A1, A2]
query<Q, P> => [A0, A2]
```

In `dens`, an `archetype` is a vector of uniquely identified `tarray`s, and a vector of entities, where each `tarray` holds a `std::vector<T>`. Each "column" represents a unique `entity` and its attached components. The sizes of all these vectors in an archetype are always equal: this is a required invariant.

```
archetype<A, B>
+-------------------------------------------+
| index     || 0 | 1 | 2 ||                 |
| entity    || e2 | e1 | e3 ||              |
| tarray<A> ||    A    |    A    |    A    ||
| tarray<B> ||   B   |   B   |   B   ||     |
+-------------------------------------------+
```

An `entity` is a strongly typed pair of IDs (identifying the `registry` and `entity` each), which also functions as a primary key into an internal database of `record`s, maintained by the `registry`. A `record` identifies an entity's owning `archetype` (if any) and its index among the columns, and is updated whenever an entity changes its archetype or is swapped with another in the same archetype (index changed). A swap-to-back-and-pop approach is used whenever columns need to be moved, minimizing the number of column adjustments (at the cost of columns being stored in an unordered fashion).

`registry` is the primary database and user-facing interface, owning all `archetype`s and `record`s. A new `record` is created for each entity, initially with no associated `archetype`. As components are attached / detached, `archetype`s are fetched / created and components added / moved as necessary. Since components are stored as `std::vector<T>`s, each `T` must be move constructible (and _will_ be relocated on archetype migration). Destroying an entity erases its corresponding column from its `archetype` (if any) and removes its `record`. Such "destroyed" entities can be reused if needed: a record will simply be recreated for the same ID<sup>**1**</sup>.

> _<sup>**1**</sup>attempting to attach components to a default constructed entity / one not owned by the registry in question will trigger an assert._

`registry::view<T...>()` returns a vector of `entity_view<T...>`, which comprises of an entity and references to its components (as `std::tuple<T&>`). This list is built by probing existing archetypes and adding the columns of those which have at least all `T...`s to the result. An optional `exclude<T...>` argument can be passed to `view()`, which will be treated as a type blocklist (archetypes that do have any of those components will be skipped).

## Contributing

Pull/merge requests are welcome.

**[Original Repository](https://github.com/karnkaul/dens)**
