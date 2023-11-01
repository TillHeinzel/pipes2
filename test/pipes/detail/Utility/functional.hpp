#pragma once

namespace pipes::detail
{
  auto negate(auto f)
  {
    return [f](auto&&... ts) mutable PIPES_RETURN(!f(PIPES_FWD(ts)...));
  }
} // namespace pipes::detail

namespace pipes::detail
{
  auto curry(auto f, auto val)
  {
    return
      [val, f](auto&&... ts) mutable PIPES_RETURN(f(val, PIPES_FWD(ts)...));
  }

  template<class T>
  auto curry(auto f, std::reference_wrapper<T> val_ref)
  {
    auto& val = val_ref.get();
    return
      [&val, f](auto&&... ts) mutable PIPES_RETURN(f(val, PIPES_FWD(ts)...));
  }

  template<class T>
  auto curry(auto f, std::reference_wrapper<const T> val_ref)
  {
    auto const& val = val_ref.get();
    return
      [&val, f](auto&&... ts) mutable PIPES_RETURN(f(val, PIPES_FWD(ts)...));
  }

  auto curry(auto f, auto x, auto... xs) { return curry(curry(f, x), xs...); }
} // namespace pipes::detail

namespace pipes::detail
{
  auto chain(auto&& x) { return PIPES_FWD(x); }

  auto chain(auto&& x, auto&& f)
  {
    return std::invoke(PIPES_FWD(f), PIPES_FWD(x));
  }

  auto chain(auto&& x, auto&& f1, auto&&... fs)
  {
    return chain(chain(PIPES_FWD(x), PIPES_FWD(f1)), PIPES_FWD(fs)...);
  }
} // namespace pipes::detail
