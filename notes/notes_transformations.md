## Viewing

(x, y, z) model coordinates -> append w (1.0) -> perf user transformations -> (x, y, z, w) s.t dividing by w
gives NDC's -> i.e x, y, z in [-1, 1] -> OpenGL clipping and rasterisation of the NDC's onto the 
viewport.

**User Transformations**:
INT --(object coords)--> Model transform (scale, rotate, translate) --(world coords)--> View transform (orient infront
of the view) --(eye coords)--> Projection transform (apply perspective proj) --(homogeneous normalised clip 
coordinates) --> OUT

frustum shaped viewing volume defined by 4 planes of the viewing frustum and the near & far plane.
orthographic viewing model: just flattening out 1 dimension.

--(model coords)--> vertex --> tessellation --> geometry --(clip coordinates) --> ... --> Rasteriser
                                                                |
                                                                v
                                                            Transform feedback

Homogeneous coords:
4 dimensional coords obtained by adding a homogeneous coord to 3 dim vectors.
gives the ability to perf rotations, translations, scaling and projective transformations.
- makes these affine transformations linear
- represents a direction
- add a 4th component of 1.0 and divide by it when moving back

- eye space: right handed
- NDC uses left handed
- so [-n, -f] mapped to [-1, 1]
- [l, r] to [-1, 1] and [b, t] to [-1, 1]

