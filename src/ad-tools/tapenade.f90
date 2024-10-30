function strainenergy_tapenade(e_sym, lambda, mu) result(energy)
  implicit none
  real(8), intent(in) :: e_sym(6)
  real(8), intent(in) :: lambda, mu
  real(8) :: energy
  energy = sum(e_sym) * lambda * mu
end function strainenergy_tapenade

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
