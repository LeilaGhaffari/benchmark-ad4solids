models=(
    elasticity-neo-hookean-current
    elasticity-neo-hookean-initial
    elasticity-neo-hookean-current-ad-adolc
    elasticity-neo-hookean-initial-ad-adolc
    elasticity-neo-hookean-current-ad-enzyme
    elasticity-neo-hookean-initial-ad-enzyme
)

for M in "${models[@]}"; do
    for n in 10; do
        build/ex02-quasistatic \
        -model $M \
        -dm_plex_box_faces $n,$n,$n \
        -dm_plex_simplex 0 \
        -dm_plex_dim 3 \
        -E 1 \
        -nu .3 \
        -order 2 \
        -bc_clamp 6 \
        -bc_traction 5 \
        -bc_traction_5 0,0,-0.1 \
        -dm_view \
        -ksp_monitor \
        -ksp_rtol 1e-3 \
        -ksp_converged_maxits \
        -ksp_converged_reason \
        -ksp_view_singularvalues \
        -mg_levels_ksp_max_it 2 \
        -snes_converged_reason \
        -snes_monitor \
        -ts_dt 1 \
        -preload \
        -log_view >results/$M-size$n-free.log;
    done;
done;
