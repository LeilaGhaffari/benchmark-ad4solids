export OMP_NUM_THREADS=1 p=62
for M in elasticity-neo-hookean-current elasticity-neo-hookean-current-ad elasticity-neo-hookean-initial elasticity-neo-hookean-initial-ad; do
    for n in {2..8}; do
        mpiexec -n $p -bind-to core \
        build/ex02-quasistatic \
        -model $M \
        -dm_plex_tps_extent $n,$n,$n \
        -ceed /cpu/self/avx \
        -E 1 \
        -nu .3 \
        -order 2 \
        -bc_clamp 1 \
        -bc_traction 2 \
        -bc_traction_2 -.02,0,0 \
        -dm_plex_shape schwarz_p \
        -dm_plex_tps_layers 2 \
        -dm_plex_tps_refine 2 \
        -dm_plex_tps_thickness .2 \
        -dm_view \
        -ksp_monitor \
        -ksp_rtol 1e-3 \
        -ksp_converged_maxits \
        -ksp_converged_reason \
        -ksp_view_singularvalues \
	    -coarse_dm_mat_type hypre \
        -mg_coarse_pc_hypre_boomeramg_coarsen_type pmis \
        -mg_coarse_pc_hypre_boomeramg_interp_type ext+i \
        -mg_coarse_pc_hypre_boomeramg_no_CF \
        -mg_coarse_pc_hypre_boomeramg_P_max 6 \
        -mg_coarse_pc_hypre_boomeramg_relax_type_down Chebyshev \
        -mg_coarse_pc_hypre_boomeramg_relax_type_up Chebyshev \
        -mg_coarse_pc_hypre_boomeramg_strong_threshold 0.5 \
        -mg_coarse_pc_type hypre \
        -mg_levels_ksp_max_it 2 \
        -snes_converged_reason \
        -snes_monitor \
        -ts_dt 1 \
        -preload \
        -dm_mat_type aij \
        -pc_type lu \
        -log_view >$M-size$n-sparse.log;
    done;
done;

