# Naming convention for matrices and points

### Use `from` in matrices
 * `model` is now `world_from_local`
 * `view` is now `view_from_world`
 * `projection` is now `clip_from_view`

### Use `space` first in points
 * `pos` can become `local_pos`
 * `pos` can become `world_pos`
 * `pos` can become `view_pos`
 * `pos` can become `clip_pos`

### Example
`clip_from_view * view_from_world * world_from_local * local_pos`
vs
`projection * view * model * pos`


