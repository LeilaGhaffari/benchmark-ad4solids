function func(x) result(res)
  implicit none
  real(4), intent(in) :: x
  real(4) :: res
  integer :: a

  a = 3
  res = a * x * x
end function func
