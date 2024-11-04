! Subroutines for computing strain energy and its derivatives
subroutine Log1pSeries(x, log1p_value)
  implicit none
  real(8), intent(in) :: x
  real(8), intent(out) :: log1p_value
  real(8) :: sum, y, y2
  integer :: i
  sum = 0.0d0
  y = x / (2.0d0 + x)
  y2 = y * y
  sum = sum + y
  do i = 0, 4
    y = y * y2
    sum = sum + y / (2 * i + 3)
  end do
  log1p_value = 2.0d0 * sum
end subroutine Log1pSeries

subroutine MatDetAM1Symmetric(A_sym, det_val)
  implicit none
  real(8), intent(in) :: A_sym(6)
  real(8), intent(out) :: det_val
  det_val = A_sym(1) * (A_sym(2) * A_sym(3) - A_sym(4) * A_sym(4)) + &
            A_sym(6) * (A_sym(4) * A_sym(5) - A_sym(6) * A_sym(3)) + &
            A_sym(5) * (A_sym(6) * A_sym(4) - A_sym(5) * A_sym(2)) + &
            A_sym(1) + A_sym(2) + A_sym(3) + &
            A_sym(1) * A_sym(2) + A_sym(1) * A_sym(3) + A_sym(2) * A_sym(3) - &
            A_sym(6) * A_sym(6) - A_sym(5) * A_sym(5) - A_sym(4) * A_sym(4)
end subroutine MatDetAM1Symmetric

subroutine MatTraceSymmetric(A_sym, trace_val)
  implicit none
  real(8), intent(in) :: A_sym(6)
  real(8), intent(out) :: trace_val
  trace_val = A_sym(1) + A_sym(2) + A_sym(3)
end subroutine MatTraceSymmetric

subroutine strainenergy_tapenade(e_sym, lambda, mu, energy)
  implicit none
  real(8), intent(in) :: e_sym(6)
  real(8), intent(in) :: lambda, mu
  real(8), intent(out) :: energy
  real(8) :: e2_sym(6), detbm1, J, logJ2, logJ, trace_e
  integer :: i
  do i = 1, 6
    e2_sym(i) = 2. * e_sym(i)
  end do
  call MatDetAM1Symmetric(e2_sym, detbm1)
  J = sqrt(detbm1 + 1)
  call Log1pSeries(detbm1, logJ2)
  logJ = logJ2 / 2.0
  call MatTraceSymmetric(e_sym, trace_e)
  energy = lambda * (J * J - 1.0) / 4.0 - lambda * logJ / 2. + mu * (-logJ + trace_e)
end subroutine strainenergy_tapenade

subroutine kirchhofftau_tapenade(lambda, mu, e_sym, tau_sym)
  implicit none
  real(8), intent(in) :: lambda, mu
  real(8), intent(in) :: e_sym(6)
  real(8), intent(out) :: tau_sym(6)
  integer :: i
  do i = 1, 6
    tau_sym(i) = (lambda + mu) * e_sym(i)
  end do
end subroutine kirchhofftau_tapenade

subroutine dtau_fwd_tapenade(lambda, mu, e_sym, de_sym, tau_sym, dtau_sym)
  implicit none
  real(8), intent(in) :: lambda, mu
  real(8), intent(in) :: e_sym(6), de_sym(6)
  real(8), intent(out) :: tau_sym(6), dtau_sym(6)
  integer :: i
  do i = 1, 6
    tau_sym(i) = lambda * e_sym(i) + mu
    dtau_sym(i) = 2.0 * de_sym(i)
  end do
end subroutine dtau_fwd_tapenade
