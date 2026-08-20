// This version of jc_voronoi.h has been modified by Seb James. jcv_point has been
// changed (from a struct of two jcv_reals to a sm::vec<jcv_real, 3> allowing the
// formation of a '2.5D' Voronoi surface)

// Copyright (c) 2015-2023 Mathias Westerdahl
// For LICENSE (MIT), USAGE or HISTORY, see bottom of file

// This version has been re-written by Seb into a templated C++ style, to allow easy choice between
// the use of single or double precision floating point values (the type T). It has also been
// namespaced (jcv) so that type names look like jcv::site instead of jcv_site; jcv::edge rather
// than jcv_edge, and so on.

// The structs and functions in the namespace sm::jcv are exported as the module sm.jcv.

module;

#include <stdlib.h> // qsort(), malloc() & free(). Seb hasn't altered the C-style memory allocation
                    // used when diagrams are generated
#include <iostream> // one sanity message
#include <limits>
#include <cmath>
#include <cassert>  // assert()
#include <cstdint>  // uintptr_t etc
#include <cstring>  // std::memset
#include <bitset>
#include <functional>

export module sm.jcv;

export import sm.mathconst;
export import sm.vec;
import sm.geometry;
import sm.winder;

export namespace sm::jcv
{
    // Seb has replaced the original point struct with use of his fixed-size mathematical vector
    // class, sm::vec
    template<typename T>
    using point = sm::vec<T, 3>;

    // forward-declare graphedge
    template <typename T> struct graphedge;

    template<typename T>
    struct site
    {
        point<T>      p;
        int           index;  // Index into the original list of points
        graphedge<T>* edges;  // The half edges owned by the cell
    };

    // The coefficients a, b and c are from the general line equation: ax * by + c = 0
    template<typename T>
    struct edge
    {
        struct edge<T>* next;
        site<T>*        sites[2];
        point<T>        pos[2];
        T               a;
        T               b;
        T               c;
    };

    template<typename T>
    struct graphedge
    {
        struct graphedge<T>* next;
        struct edge<T>*      edge_;
        struct site<T>*      neighbor;
        point<T>             pos[2];
        T                    angle;
    };

    template<typename T>
    struct delaunay_iter
    {
        const edge<T>* sentinel;
        const edge<T>* current;
    };

    template<typename T>
    struct delaunay_edge
    {
        const edge<T>* edge_;      // The voronoi edge separating the two sites
        const site<T>* sites[2];
        point<T>       pos[2];     // the positions of the two sites
    };

    template<typename T>
    struct rect
    {
        point<T> min;
        point<T> max;
    };

    // Forward declare context_internal for the std::function
    template<typename T> struct context_internal;

    template<typename T>
    struct clipper
    {
        // Tests if a point is inside the final shape
        std::function<int(const clipper<T>* _clipper, const point<T> p)> test_fn;
        // Given an edge, and the clipper, calculates the e->pos[0] and e->pos[1]
        // Returns 0 if not successful
        std::function<int(const clipper<T>* _clipper, edge<T>* e)>       clip_fn;
        // Given the clipper, the site and the last edge,
        // closes any gaps in the polygon by adding new edges that follow the bounding shape
        // The internal context is use when allocating new edges.
        std::function<void(const clipper<T>* _clipper,
                           context_internal<T>* allocator, site<T>* s)>  fill_fn;

        point<T>     min;        // The bounding rect min
        point<T>     max;        // The bounding rect max
        void*        ctx;        // User defined context function
    };

    // Second batch of structs
    template<typename T>
    struct halfedge
    {
        edge<T>*            edge_;
        struct halfedge<T>* left;
        struct halfedge<T>* right;
        point<T>            vertex;
        T                   y;
        int                 direction; // 0=left, 1=right
        int                 pqpos;
    };

    struct memoryblock
    {
        size_t              sizefree;
        struct memoryblock* next;
        char*               memory;
    };

    struct priorityqueue
    {
        // Implements a binary heap
        int    maxnumitems;
        int    numitems;
        void** items;
    };

    using FJCVAllocFn = void*(void* userctx, size_t size);
    using FJCVFreeFn = void(void* userctx, void* p);

    template<typename T>
    struct context_internal
    {
        void*          mem;
        edge<T>*       edges;
        halfedge<T>*   beachline_start;
        halfedge<T>*   beachline_end;
        halfedge<T>*   last_inserted;
        priorityqueue* eventqueue;

        site<T>*       sites;
        site<T>*       bottomsite;
        int            numsites;
        int            currentsite;
        int            _padding;

        memoryblock*   memblocks;
        edge<T>*       edgepool;
        halfedge<T>*   halfedgepool;
        void**         eventmem;
        clipper<T>     clipper_;

        void*                      memctx; // Given by the user
        std::function<FJCVAllocFn> alloc;
        std::function<FJCVFreeFn>  free;

        rect<T>        rect_;
    };

    template<typename T>
    struct diagram
    {
        context_internal<T>* internal;
        int                  numsites;
        point<T>             min;
        point<T>             max;
    };

    // Used for boundary clipping
    template<typename T>
    struct clipping_polygon
    {
        std::vector<jcv::point<T>> points;
    };

    // The mananger class. Type T is what is called real in the original code
    template<typename T> requires std::is_floating_point_v<T>
    struct manager
    {
        manager(){}
        ~manager() { jcv::manager<T>::diagram_free (&this->diagram); }

        static constexpr T edge_intersect_threshold = T{1.0e-10}; // JCV_EDGE_INTERSECT_THRESHOLD: Fix for Issue #40

        // INTERNAL FUNCTIONS

        static const int DIRECTION_LEFT  = 0;
        static const int DIRECTION_RIGHT = 1;

        static constexpr T invalid_value = std::numeric_limits<T>::lowest();

        // App specific equality of scalars
        static int equal (T a, T b) { return std::abs(a - b) < std::numeric_limits<T>::epsilon(); }

        // Vector equality
        static int equal (const point<T>* pt1, const point<T>* pt2)
        {
            return equal (pt1->y(), pt2->y()) && equal (pt1->x(), pt2->x());
        }

        // point (i.e. vector) comparison, accepting void pointers
        static int point_cmp (const void* p1, const void* p2)
        {
            const point<T>* s1 = static_cast<const point<T>*>(p1);
            const point<T>* s2 = static_cast<const point<T>*>(p2);
            return (s1->y() != s2->y()) ? (s1->y() < s2->y() ? -1 : 1) : (s1->x() < s2->x() ? -1 : 1);
        }

        // Return app-specific pt1 < pt2 operation between vectors
        static int lessthan (const point<T>* pt1, const point<T>* pt2)
        {
            return (pt1->y() == pt2->y()) ? (pt1->x() < pt2->x()) : pt1->y() < pt2->y();
        }

        // edges and corners
        static const int EDGE_LEFT    = 1;
        static const int EDGE_RIGHT   = 2;
        static const int EDGE_BOTTOM  = 4;
        static const int EDGE_TOP     = 8;

        static const int CORNER_NONE          = 0;
        static const int CORNER_TOP_LEFT      = 1;
        static const int CORNER_BOTTOM_LEFT   = 2;
        static const int CORNER_BOTTOM_RIGHT  = 3;
        static const int CORNER_TOP_RIGHT     = 4;

        static int get_edge_flags (const point<T>* pt, const point<T>* min, const point<T>* max)
        {
            int flags = 0;
            if (pt->x() == min->x()) {
                flags |= EDGE_LEFT;
            } else if (pt->x() == max->x()) {
                flags |= EDGE_RIGHT;
            }
            if (pt->y() == min->y()) {
                flags |= EDGE_BOTTOM;
            } else if (pt->y() == max->y()) {
                flags |= EDGE_TOP;
            }
            return flags;
        }

        static int edge_flags_to_corner (int edge_flags)
        {
#define TEST_FLAGS(_FLAGS, _RETVAL) if ( (_FLAGS) == edge_flags) return _RETVAL
            TEST_FLAGS (EDGE_TOP|EDGE_LEFT, CORNER_TOP_LEFT);
            TEST_FLAGS (EDGE_TOP|EDGE_RIGHT, CORNER_TOP_RIGHT);
            TEST_FLAGS (EDGE_BOTTOM|EDGE_LEFT, CORNER_BOTTOM_LEFT);
            TEST_FLAGS (EDGE_BOTTOM|EDGE_RIGHT, CORNER_BOTTOM_RIGHT);
#undef TEST_FLAGS
            return 0;
        }

        [[maybe_unused]] static int is_corner(int corner) { return corner != 0; }

        static int corner_rotate_90 (int corner)
        {
            corner--;
            corner = (corner+1)%4;
            return corner + 1;
        }

        static point<T> corner_to_point (int corner, const point<T>* min, const point<T>* max)
        {
            point<T> p;
            if      (corner == CORNER_TOP_LEFT)     { p[0] = min->x(); p[1] = max->y(); }
            else if (corner == CORNER_TOP_RIGHT)    { p[0] = max->x(); p[1] = max->y(); }
            else if (corner == CORNER_BOTTOM_LEFT)  { p[0] = min->x(); p[1] = min->y(); }
            else if (corner == CORNER_BOTTOM_RIGHT) { p[0] = max->x(); p[1] = min->y(); }
            else { p[0] = invalid_value; p[1] = invalid_value; }
            return p;
        }

        static T point_dist_sq (const point<T>* pt1, const point<T>* pt2)
        {
            T diffx = pt1->x() - pt2->x();
            T diffy = pt1->y() - pt2->y();
            return diffx * diffx + diffy * diffy;
        }

        static T point_dist (const point<T>* pt1, const point<T>* pt2)
        {
            return std::sqrt (point_dist_sq (pt1, pt2));
        }

        // Uses free (or the registered custom free function)
        static void diagram_free (diagram<T>* d)
        {
            context_internal<T>* internal = d->internal;
            void* memctx = internal->memctx;
            while (internal->memblocks) {
                memoryblock* p = internal->memblocks;
                internal->memblocks = internal->memblocks->next;
                internal->free (memctx, p);
            }

            internal->free (memctx, internal->mem);
        }

        // Returns an array of sites, where each index is the same as the original input point array.
        static const site<T>* diagram_get_sites (const diagram<T>* diagram)
        {
            return diagram->internal->sites;
        }

        // User API
        const site<T>* diagram_get_sites() { return diagram_get_sites (&this->diagram); }

        // Iterates over a list of edges, skipping invalid edges (where p0==p1)
        const edge<T>* diagram_get_next_edge (const edge<T>* _edge)
        {
            const edge<T>* e = _edge->next;
            while (e != 0 && equal (&e->pos[0], &e->pos[1])) {
                e = e->next;
            }
            return e;
        }

        // Returns a linked list of all the voronoi edges excluding the ones that lie on the borders of
        // the bounding box.  For a full list of edges, you need to iterate over the sites, and their
        // graph edges.
        const edge<T>* diagram_get_edges (const diagram<T>* diagram)
        {
            edge<T> e;
            e.next = diagram->internal->edges;
            return diagram_get_next_edge (&e);
        }

        // Used by delaunay code
        delaunay_iter<T> d_iter = {};

        // Creates an iterator over the delaunay edges of a voronoi diagram
        void delaunay_begin()
        {
            this->d_iter.current = nullptr;
            this->d_iter.sentinel = diagram_get_edges (&this->diagram);
        }

        // Steps the iterator and returns the next edge Returns 0 when there are no more edges
        int delaunay_next (delaunay_edge<T>* next)
        {
            if (this->d_iter.sentinel) {
                this->d_iter.current = this->d_iter.sentinel;
                this->d_iter.sentinel = 0;
            } else {
                // Note: If we use the raw edges, we still get a proper delaunay triangulation
                // However, the result looks less relevant to the cells contained within the bounding box
                // E.g. some cells that look isolated from each other, suddenly still are connected,
                // because they share an edge outside of the bounding box
                this->d_iter.current = diagram_get_next_edge (this->d_iter.current);
            }

            while (this->d_iter.current && (this->d_iter.current->sites[0] == 0 || this->d_iter.current->sites[1] == 0)) {
                this->d_iter.current = diagram_get_next_edge (this->d_iter.current);
            }

            if (!this->d_iter.current) { return 0; }

            next->edge_ = this->d_iter.current;
            next->sites[0] = next->edge_->sites[0];
            next->sites[1] = next->edge_->sites[1];
            next->pos[0] = next->sites[0]->p;
            next->pos[1] = next->sites[1]->p;
            return 1;
        }

        static void* align (void* value, size_t alignment)
        {
            return (void*) (((uintptr_t) value + (alignment-1)) & ~(alignment-1));
        }

        static void* alloc (context_internal<T>* internal, size_t size)
        {
            if (!internal->memblocks || internal->memblocks->sizefree < (size + sizeof(void*))) {
                size_t blocksize = 16 * 1024;
                memoryblock* block = (memoryblock*)internal->alloc (internal->memctx, blocksize);
                size_t offset = sizeof(memoryblock);
                block->sizefree = blocksize - offset;
                block->next = internal->memblocks;
                block->memory = ((char*)block) + offset;
                internal->memblocks = block;
            }
            void* p_raw = internal->memblocks->memory;
            void* p_aligned = align (p_raw, sizeof(void*));
            size += (uintptr_t)p_aligned - (uintptr_t)p_raw;
            internal->memblocks->memory += size;
            internal->memblocks->sizefree -= size;
            return p_aligned;
        }

        static edge<T>* alloc_edge (context_internal<T>* internal)
        {
            return (edge<T>*) alloc (internal, sizeof(edge<T>));
        }

        static halfedge<T>* alloc_halfedge (context_internal<T>* internal)
        {
            if (internal->halfedgepool) {
                halfedge<T>* edge = internal->halfedgepool;
                internal->halfedgepool = internal->halfedgepool->right;
                return edge;
            }
            return (halfedge<T>*) alloc (internal, sizeof(halfedge<T>));
        }

        static graphedge<T>* alloc_graphedge (context_internal<T>* internal)
        {
            return (graphedge<T>*) alloc (internal, sizeof(graphedge<T>));
        }

        static void* alloc_fn (void* memctx, size_t size)
        {
            (void)memctx;
            return malloc (size);
        }

        static void free_fn (void* memctx, void* p)
        {
            (void)memctx;
            free (p);
        }

        // edge methods
        static int is_valid (const point<T>* p)
        {
            return (p->x() != invalid_value || p->y() != invalid_value) ? 1 : 0;
        }

        static void edge_create (edge<T>* e, site<T>* s1, site<T>* s2)
        {
            e->next = 0;
            e->sites[0] = s1;
            e->sites[1] = s2;
            e->pos[0][0] = invalid_value;
            e->pos[0][1] = invalid_value;
            e->pos[1][0] = invalid_value;
            e->pos[1][1] = invalid_value;

            // Create line equation between S1 and S2:
            // T a = -1 * (s2->p[1] - s1->p[1]);
            // T b = s2->p[0] - s1->p[0];
            // //T c = -1 * (s2->p[0] - s1->p[0]) * s1->p[1] + (s2->p[1] - s1->p[1]) * s1->p[0];
            //
            // // create perpendicular line
            // T pa = b;
            // T pb = -a;
            // //T pc = pa * s1->p[0] + pb * s1->p[1];
            //
            // // Move to the mid point
            // T mx = s1->p[0] + dx * T(0.5);
            // T my = s1->p[1] + dy * T(0.5);
            // T pc =  (pa * mx + pb * my);

            T dx = s2->p[0] - s1->p[0];
            T dy = s2->p[1] - s1->p[1];
            int dx_is_larger = (dx*dx) > (dy*dy); // instead of fabs

            // Simplify it, using dx and dy
            e->c = dx * (s1->p[0] + dx * T{0.5}) + dy * (s1->p[1] + dy * T{0.5});

            if (dx_is_larger) {
                e->a = T{1};
                e->b = dy / dx;
                e->c /= dx;
            } else {
                e->a = dx / dy;
                e->b = T{1};
                e->c /= dy;
            }
        }

        // CLIPPING
        static int boxshape_test (const clipper<T>* clipper, const point<T> p)
        {
            return p[0] >= clipper->min[0] && p[0] <= clipper->max[0]
            && p[1] >= clipper->min[1] && p[1] <= clipper->max[1];
        }

        // The line equation: ax + by + c = 0
        // see edge_create
        static int boxshape_clip (const clipper<T>* clipper, edge<T>* e)
        {
            T pxmin = clipper->min[0];
            T pxmax = clipper->max[0];
            T pymin = clipper->min[1];
            T pymax = clipper->max[1];

            T x1, y1, x2, y2;
            point<T>* s1;
            point<T>* s2;
            if (e->a == T{1} && e->b >= T{0}) {
                s1 = is_valid(&e->pos[1]) ? &e->pos[1] : 0;
                s2 = is_valid(&e->pos[0]) ? &e->pos[0] : 0;
            } else {
                s1 = is_valid(&e->pos[0]) ? &e->pos[0] : 0;
                s2 = is_valid(&e->pos[1]) ? &e->pos[1] : 0;
            }

            if (e->a == T{1}) { // delta x is larger
                y1 = pymin;
                if (s1 != 0 && s1->y() > pymin) { y1 = s1->y(); }
                if (y1 > pymax) { y1 = pymax; }
                x1 = e->c - e->b * y1;
                y2 = pymax;
                if (s2 != 0 && s2->y() < pymax) { y2 = s2->y(); }

                if (y2 < pymin) { y2 = pymin; }
                x2 = (e->c) - (e->b) * y2;
                // Never occurs according to lcov
                // if (((x1 > pxmax) & (x2 > pxmax)) | ((x1 < pxmin) & (x2 < pxmin)))
                // {
                //     return 0;
                // }
                if (x1 > pxmax) {
                    x1 = pxmax;
                    y1 = (e->c - x1) / e->b;
                } else if (x1 < pxmin) {
                    x1 = pxmin;
                    y1 = (e->c - x1) / e->b;
                }
                if (x2 > pxmax) {
                    x2 = pxmax;
                    y2 = (e->c - x2) / e->b;
                } else if (x2 < pxmin) {
                    x2 = pxmin;
                    y2 = (e->c - x2) / e->b;
                }

            } else { // delta y is larger

                x1 = pxmin;
                if (s1 != 0 && s1->x() > pxmin) { x1 = s1->x(); }
                if (x1 > pxmax) { x1 = pxmax; }
                y1 = e->c - e->a * x1;
                x2 = pxmax;
                if (s2 != 0 && s2->x() < pxmax) { x2 = s2->x(); }
                if (x2 < pxmin) { x2 = pxmin; }
                y2 = e->c - e->a * x2;
                // Never occurs according to lcov
                // if (((y1 > pymax) & (y2 > pymax)) | ((y1 < pymin) & (y2 < pymin))) { return 0; }
                if (y1 > pymax) {
                    y1 = pymax;
                    x1 = (e->c - y1) / e->a;
                } else if (y1 < pymin) {
                    y1 = pymin;
                    x1 = (e->c - y1) / e->a;
                }
                if (y2 > pymax) {
                    y2 = pymax;
                    x2 = (e->c - y2) / e->a;
                } else if (y2 < pymin) {
                    y2 = pymin;
                    x2 = (e->c - y2) / e->a;
                }
            }

            e->pos[0][0] = x1;
            e->pos[0][1] = y1;
            e->pos[1][0] = x2;
            e->pos[1][1] = y2;

            // If the two points are equal, the result is invalid
            return (x1 == x2 && y1 == y2) ? 0 : 1;
        }

        // The line equation: ax + by + c = 0
        // see edge_create
        static int edge_clipline (context_internal<T>* internal, edge<T>* e)
        {
            return internal->clipper_.clip_fn (&internal->clipper_, e);
        }

        static edge<T>* edge_new (context_internal<T>* internal, site<T>* s1, site<T>* s2)
        {
            edge<T>* e = alloc_edge (internal);
            edge_create (e, s1, s2);
            return e;
        }


        // halfedge

        static void halfedge_link (halfedge<T>* edge, halfedge<T>* newedge)
        {
            newedge->left = edge;
            newedge->right = edge->right;
            edge->right->left = newedge;
            edge->right = newedge;
        }

        static void halfedge_unlink (halfedge<T>* he)
        {
            he->left->right = he->right;
            he->right->left = he->left;
            he->left  = 0;
            he->right = 0;
        }

        static halfedge<T>* halfedge_new (context_internal<T>* internal, edge<T>* e, int direction)
        {
            halfedge<T>* he = alloc_halfedge (internal);
            he->edge_       = e;
            he->left        = 0;
            he->right       = 0;
            he->direction   = direction;
            he->pqpos       = 0;
            // These are set outside
            //he->y()
            //he->vertex
            return he;
        }

        static void halfedge_delete (context_internal<T>* internal, halfedge<T>* he)
        {
            he->right = internal->halfedgepool;
            internal->halfedgepool = he;
        }

        static site<T>* halfedge_leftsite (const halfedge<T>* he)
        {
            return he->edge_->sites[he->direction];
        }

        static site<T>* halfedge_rightsite (const halfedge<T>* he)
        {
            return he->edge_ ? he->edge_->sites[1 - he->direction] : 0;
        }

        static int halfedge_rightof (const halfedge<T>* he, const point<T>* p)
        {
            const edge<T>* e = he->edge_;
            const site<T>* topsite = e->sites[1];

            int right_of_site = (p->x() > topsite->p[0]) ? 1 : 0;
            if (right_of_site && he->direction == DIRECTION_LEFT) { return 1; }
            if (!right_of_site && he->direction == DIRECTION_RIGHT) { return 0; }

            T dxp, dyp, dxs, t1, t2, t3, yl;

            int above;
            if (e->a == T{1}) {
                dyp = p->y() - topsite->p[1];
                dxp = p->x() - topsite->p[0];
                int fast = 0;
                if ((!right_of_site & (e->b < T{0})) | (right_of_site & (e->b >= T{0}))) {
                    above = dyp >= e->b * dxp;
                    fast = above;
                } else {
                    above = (p->x() + p->y() * e->b) > e->c;
                    if (e->b < T{0}) { above = !above; }
                    if (!above) { fast = 1; }
                }
                if (!fast) {
                    dxs = topsite->p[0] - e->sites[0]->p[0];
                    above = e->b * (dxp * dxp - dyp * dyp)
                    < dxs * dyp * (T{1} + T{2} * dxp / dxs + e->b * e->b);
                    if (e->b < T{0}) { above = !above; }
                }
            } else { // e->b == 1
                yl = e->c - e->a * p->x();
                t1 = p->y() - yl;
                t2 = p->x() - topsite->p[0];
                t3 = yl - topsite->p[1];
                above = t1 * t1 > (t2 * t2 + t3 * t3);
            }
            return (he->direction == DIRECTION_LEFT ? above : !above);
        }

        // Keeps the priority queue sorted with events sorted in ascending order
        // Return 1 if the edges needs to be swapped
        static int halfedge_compare (const halfedge<T>* he1, const halfedge<T>* he2)
        {
            return  (he1->y == he2->y) ? he1->vertex[0] > he2->vertex[0] : he1->y > he2->y;
        }

        static int halfedge_intersect (const halfedge<T>* he1, const halfedge<T>* he2, point<T>* out)
        {
            const edge<T>* e1 = he1->edge_;
            const edge<T>* e2 = he2->edge_;

            T d = e1->a * e2->b - e1->b * e2->a;
            if (-edge_intersect_threshold < d && d < edge_intersect_threshold) { return 0; }
            (*out)[0] = (e1->c * e2->b - e1->b * e2->c) / d;
            (*out)[1] = (e1->a * e2->c - e1->c * e2->a) / d;
            // I considered trying to determine the correct z here, but we don't have all the
            // information required. So just set out->z to a default value meaning 'unset' (Seb)
            (*out)[2] = 0.0f; // NB: this does not set z for all edges

            const edge<T>* e;
            const halfedge<T>* he;
            if (lessthan (&e1->sites[1]->p, &e2->sites[1]->p)) {
                he = he1;
                e = e1;
            } else {
                he = he2;
                e = e2;
            }

            int right_of_site = out->x() >= e->sites[1]->p[0];
            if ((right_of_site && he->direction == DIRECTION_LEFT)
                || (!right_of_site && he->direction == DIRECTION_RIGHT)) {
                return 0;
            }

            return 1;
        }


        // Priority queue

        static int pq_moveup (priorityqueue* pq, int pos)
        {
            halfedge<T>** items = (halfedge<T>**)pq->items;
            halfedge<T>* node = items[pos];

            for (int parent = (pos >> 1); pos > 1 && halfedge_compare(items[parent], node); pos = parent, parent = parent >> 1) {
                items[pos] = items[parent];
                items[pos]->pqpos = pos;
            }

            node->pqpos = pos;
            items[pos] = node;
            return pos;
        }

        static int pq_maxchild (priorityqueue* pq, int pos)
        {
            int child = pos << 1;
            if (child >= pq->numitems) { return 0; }
            halfedge<T>** items = (halfedge<T>**)pq->items;
            if ((child + 1) < pq->numitems && halfedge_compare (items[child], items[child+1])) { return child + 1; }
            return child;
        }

        static int pq_movedown (priorityqueue* pq, int pos)
        {
            halfedge<T>** items = (halfedge<T>**)pq->items;
            halfedge<T>* node = items[pos];

            int child = pq_maxchild (pq, pos);
            while (child && halfedge_compare (node, items[child])) {
                items[pos] = items[child];
                items[pos]->pqpos = pos;
                pos = child;
                child = pq_maxchild(pq, pos);
            }

            items[pos] = node;
            items[pos]->pqpos = pos;
            return pos;
        }

        static void pq_create (priorityqueue* pq, int capacity, void** buffer)
        {
            pq->maxnumitems = capacity;
            pq->numitems    = 1;
            pq->items       = buffer;
        }

        static int pq_empty (priorityqueue* pq) { return pq->numitems == 1 ? 1 : 0; }

        static int pq_push (priorityqueue* pq, void* node)
        {
            assert (pq->numitems < pq->maxnumitems);
            int n = pq->numitems++;
            pq->items[n] = node;
            return pq_moveup(pq, n);
        }

        static void* pq_pop (priorityqueue* pq)
        {
            void* node = pq->items[1];
            pq->items[1] = pq->items[--pq->numitems];
            pq_movedown(pq, 1);
            return node;
        }

        static void* pq_top (priorityqueue* pq) { return pq->items[1]; }

        static void pq_remove (priorityqueue* pq, halfedge<T>* node)
        {
            if (pq->numitems == 1) { return; }
            int pos = node->pqpos;
            if (pos == 0) { return; }

            halfedge<T>** items = (halfedge<T>**)pq->items;

            items[pos] = items[--pq->numitems];
            if (halfedge_compare (node, items[pos])) {
                pq_moveup (pq, pos);
            } else {
                pq_movedown (pq, pos);
            }
            node->pqpos = pos;
        }

        // internal functions

        static site<T>* nextsite (context_internal<T>* internal)
        {
            return (internal->currentsite < internal->numsites) ? &internal->sites[internal->currentsite++] : 0;
        }

        static halfedge<T>* get_edge_above_x (context_internal<T>* internal, const point<T>* p)
        {
            // Gets the arc on the beach line at the x coordinate (i.e. right above the new site event)

            // A good guess it's close by (Can be optimized)
            halfedge<T>* he = internal->last_inserted;
            if (!he) {
                if (p->x() < (internal->rect_.max[0] - internal->rect_.min[0]) / 2) { he = internal->beachline_start; }
                else { he = internal->beachline_end; }
            }

            if (he == internal->beachline_start || (he != internal->beachline_end && halfedge_rightof(he, p))) {
                do { he = he->right; }
                while (he != internal->beachline_end && halfedge_rightof(he, p));

                he = he->left;
            } else {
                do { he = he->left; }
                while (he != internal->beachline_start && !halfedge_rightof(he, p));
            }

            return he;
        }

        static int check_circle_event (const halfedge<T>* he1, const halfedge<T>* he2, point<T>* vertex)
        {
            edge<T>* e1 = he1->edge_;
            edge<T>* e2 = he2->edge_;
            if (e1 == 0 || e2 == 0 || e1->sites[1] == e2->sites[1]) { return 0; }

            return halfedge_intersect (he1, he2, vertex);
        }

        static void site_event (context_internal<T>* internal, site<T>* _site)
        {
            halfedge<T>* left   = get_edge_above_x(internal, &_site->p);
            halfedge<T>* right  = left->right;
            site<T>*     bottom = halfedge_rightsite(left);
            if (!bottom) { bottom = internal->bottomsite; }

            edge<T>* _edge = edge_new (internal, bottom, _site);
            _edge->next = internal->edges;
            internal->edges = _edge;

            halfedge<T>* edge1 = halfedge_new (internal, _edge, DIRECTION_LEFT);
            halfedge<T>* edge2 = halfedge_new (internal, _edge, DIRECTION_RIGHT);

            halfedge_link (left, edge1);
            halfedge_link (edge1, edge2);

            internal->last_inserted = right;

            point<T> p;
            if (check_circle_event (left, edge1, &p)) {
                pq_remove (internal->eventqueue, left);
                left->vertex    = p;
                left->y         = p[1] + point_dist (&_site->p, &p);
                pq_push(internal->eventqueue, left);
            }
            if (check_circle_event (edge2, right, &p)) {
                edge2->vertex   = p;
                edge2->y        = p[1] + point_dist (&_site->p, &p);
                pq_push (internal->eventqueue, edge2);
            }
        }

        // https://cp-algorithms.com/geometry/oriented-triangle-area.html
        static T determinant (const point<T>* a, const point<T>* b, const point<T>* c)
        {
            return (b->x() - a->x()) * (c->y() - a->y()) - (b->y() - a->y())*(c->x() - a->x());
        }

        static T calc_sort_metric (const site<T>* _site, const graphedge<T>* _edge)
        {
            // We take the average of the two points, since we can better distinguish between very small edges
            T x = (_edge->pos[0][0] + _edge->pos[1][0]) * T{0.5};
            T y = (_edge->pos[0][1] + _edge->pos[1][1]) * T{0.5};
            T diffy = y - _site->p[1];
            T angle = std::atan2 (diffy, x - _site->p[0]);
            if (diffy < 0) { angle = angle + sm::mathconst<T>::two_pi; }
            return angle;
        }

        static int graphedge_eq (graphedge<T>* a, graphedge<T>* b)
        {
            return equal (a->angle, b->angle) && equal (&a->pos[0], &b->pos[0]) && equal  (&a->pos[1], &b->pos[1]);
        }

        static void sortedges_insert (site<T>* _site, graphedge<T>* _edge)
        {
            // Special case for the head end
            graphedge<T>* prev = 0;
            if (_site->edges == 0 || _site->edges->angle >= _edge->angle) {
                _edge->next = _site->edges;
                _site->edges = _edge;
            } else {
                // Locate the node before the point of insertion
                graphedge<T>* current = _site->edges;
                while (current->next != 0 && current->next->angle < _edge->angle) { current = current->next; }
                prev = current;
                _edge->next = current->next;
                current->next = _edge;
            }

            // check to avoid duplicates
            if (prev && graphedge_eq(prev, _edge)) {
                prev->next = _edge->next;
            } else if (_edge->next && graphedge_eq(_edge, _edge->next)) {
                _edge->next = _edge->next->next;
            }
        }

        static void finishline (context_internal<T>* internal, edge<T>* e)
        {
            int er = 0;
            if (!(er = edge_clipline (internal, e))) {
                return;
            } else if (er == 2) {
                // 2 means the edge 'was removed/not added'
                return;
            }

            // Make sure the graph edges are CCW
            int flip = determinant (&e->sites[0]->p, &e->pos[0], &e->pos[1]) > T{0} ? 0 : 1;

            for (int i = 0; i < 2; ++i) {
                graphedge<T>* ge = alloc_graphedge (internal);
                ge->edge_ = e;
                ge->next = 0;
                ge->neighbor = e->sites[1-i];
                ge->pos[flip] = e->pos[i];
                ge->pos[1-flip] = e->pos[1-i];
                ge->angle = calc_sort_metric(e->sites[i], ge);
                sortedges_insert (e->sites[i], ge);
            }
        }


        static void endpos (context_internal<T>* internal, edge<T>* e, const point<T>* p, int direction)
        {
            e->pos[direction] = *p;
            if (!is_valid(&e->pos[1 - direction])) { return; }
            finishline (internal, e);
        }

        static void create_corner_edge (context_internal<T>* internal, const site<T>* site, graphedge<T>* current, graphedge<T>* gap)
        {
            gap->neighbor   = 0;
            gap->pos[0]     = current->pos[1];

            if (current->pos[1][0] < internal->rect_.max[0] && current->pos[1][1] == internal->rect_.min[1]) {
                gap->pos[1][0] = internal->rect_.max[0];
                gap->pos[1][1] = internal->rect_.min[1];
            } else if (current->pos[1][0] > internal->rect_.min[0] && current->pos[1][1] == internal->rect_.max[1]) {
                gap->pos[1][0] = internal->rect_.min[0];
                gap->pos[1][1] = internal->rect_.max[1];
            } else if (current->pos[1][1] > internal->rect_.min[1] && current->pos[1][0] == internal->rect_.min[0]) {
                gap->pos[1][0] = internal->rect_.min[0];
                gap->pos[1][1] = internal->rect_.min[1];
            } else if (current->pos[1][1] < internal->rect_.max[1] && current->pos[1][0] == internal->rect_.max[0]) {
                gap->pos[1][0] = internal->rect_.max[0];
                gap->pos[1][1] = internal->rect_.max[1];
            }

            gap->angle = calc_sort_metric(site, gap);
        }

        static edge<T>* create_gap_edge (context_internal<T>* internal, site<T>* site, graphedge<T>* ge)
        {
            edge<T>* edge   = alloc_edge (internal);
            edge->pos[0]    = ge->pos[0];
            edge->pos[1]    = ge->pos[1];
            edge->sites[0]  = site;
            edge->sites[1]  = 0;
            edge->a = edge->b = edge->c = 0;
            edge->next      = internal->edges;
            internal->edges = edge;
            return edge;
        }

        static void boxshape_fill (const clipper<T>* clipper, context_internal<T>* allocator, site<T>* site)
        {
            // They're sorted CCW, so if the current->pos[1] != next->pos[0], then we have a gap
            graphedge<T>* curr_graphedge = site->edges;
            if (!curr_graphedge) {
                // No edges, then it should be a single cell
                assert (allocator->numsites == 1);

                graphedge<T>* gap = alloc_graphedge (allocator);
                gap->neighbor   = 0;
                gap->pos[0]     = clipper->min;
                gap->pos[1][0]  = clipper->max[0];
                gap->pos[1][1]  = clipper->min[1];
                gap->angle      = calc_sort_metric(site, gap);
                gap->next       = 0;
                gap->edge_      = create_gap_edge (allocator, site, gap);

                curr_graphedge = gap;
                site->edges = gap;
            }

            graphedge<T>* next = curr_graphedge->next;
            if (!next) {
                graphedge<T>* gap = alloc_graphedge (allocator);
                create_corner_edge(allocator, site, curr_graphedge, gap);
                gap->edge_ = create_gap_edge (allocator, site, gap);

                gap->next = curr_graphedge->next;
                curr_graphedge->next = gap;
                curr_graphedge = gap;
                next = site->edges;
            }

            // Need a sanity check, as this while loop can go infinite if earlier code fails to
            // generate the Voronoi diagram correctly. We usually go 5 or 6 times around the while,
            // so if we get to 1024 we're probably in an interminable loop.
            constexpr int loopcount_thresh = 1024;
            int loopcount = 0;

            while (curr_graphedge && next && loopcount < loopcount_thresh) {

                int current_edge_flags = get_edge_flags(&curr_graphedge->pos[1], &clipper->min, &clipper->max);

                if (current_edge_flags && !equal(&curr_graphedge->pos[1], &next->pos[0])) {
                    // Cases:
                    //  Current and Next on the same border
                    //  Current on one border, and Next on another border
                    //  Current on the corner, Next on the border
                    //  Current on the corner, Next on another border (another corner in between)

                    int next_edge_flags = get_edge_flags(&next->pos[0], &clipper->min, &clipper->max);
                    if (current_edge_flags & next_edge_flags) {
                        // Current and Next on the same border
                        graphedge<T>* gap = alloc_graphedge (allocator);
                        gap->neighbor   = 0;
                        gap->pos[0]     = curr_graphedge->pos[1];
                        gap->pos[1]     = next->pos[0];
                        gap->angle      = calc_sort_metric (site, gap);
                        gap->edge_      = create_gap_edge (allocator, site, gap);

                        gap->next = curr_graphedge->next;
                        curr_graphedge->next = gap;
                    } else {
                        // Current and Next on different borders
                        int corner_flag = edge_flags_to_corner (current_edge_flags);
                        if (corner_flag) {
                            // we are already at one corner, so we need to find the next one
                            corner_flag = corner_rotate_90 (corner_flag);
                        } else {
                            // we are on the middle of a border
                            // we need to find the adjacent corner, following the borders CCW
                            if      (current_edge_flags == EDGE_TOP)    { corner_flag = CORNER_TOP_LEFT; }
                            else if (current_edge_flags == EDGE_LEFT)   { corner_flag = CORNER_BOTTOM_LEFT; }
                            else if (current_edge_flags == EDGE_BOTTOM) { corner_flag = CORNER_BOTTOM_RIGHT; }
                            else if (current_edge_flags == EDGE_RIGHT)  { corner_flag = CORNER_TOP_RIGHT; }

                        }
                        point<T> corner = corner_to_point (corner_flag, &clipper->min, &clipper->max);

                        graphedge<T>* gap = alloc_graphedge (allocator);
                        gap->neighbor   = 0;
                        gap->pos[0]     = curr_graphedge->pos[1];
                        gap->pos[1]     = corner;
                        gap->angle      = calc_sort_metric(site, gap);
                        gap->edge_      = create_gap_edge (allocator, site, gap);

                        gap->next = curr_graphedge->next;
                        curr_graphedge->next = gap;
                    }
                } // else inside border

                curr_graphedge = curr_graphedge->next;
                if (curr_graphedge) {
                    next = curr_graphedge->next;
                    if (!next) { next = site->edges; }
                }
                ++loopcount;
                if (loopcount >= loopcount_thresh) {
                    std::cout << "Too many loops. This can be caused by numerical precision errors when using T==float on some sets of points\n";
                }
            }
        }


        // Since the algorithm leaves gaps at the borders/corner, we want to fill them
        static void fillgaps (diagram<T>* diagram)
        {
            context_internal<T>* internal = diagram->internal;
            if (!internal->clipper_.fill_fn) { return; }
            for (int i = 0; i < internal->numsites; ++i) {
                site<T>* site = &internal->sites[i];
                // Call fill fn for each site
                internal->clipper_.fill_fn (&internal->clipper_, internal, site);
            }
        }


        static void circle_event (context_internal<T>* internal)
        {
            halfedge<T>* left      = (halfedge<T>*)pq_pop (internal->eventqueue);
            halfedge<T>* leftleft  = left->left;
            halfedge<T>* right     = left->right;
            halfedge<T>* rightright= right->right;
            site<T>* bottom = halfedge_leftsite(left);
            site<T>* top    = halfedge_rightsite(right);

            point<T> vertex = left->vertex;
            endpos (internal, left->edge_, &vertex, left->direction);
            endpos (internal, right->edge_, &vertex, right->direction);

            internal->last_inserted = rightright;

            pq_remove (internal->eventqueue, right);
            halfedge_unlink (left);
            halfedge_unlink (right);
            halfedge_delete (internal, left);
            halfedge_delete (internal, right);

            int direction = DIRECTION_LEFT;
            if (bottom->p[1] > top->p[1]) {
                site<T>* temp = bottom;
                bottom = top;
                top = temp;
                direction = DIRECTION_RIGHT;
            }

            edge<T>* edge = edge_new (internal, bottom, top);
            edge->next = internal->edges;
            internal->edges = edge;

            halfedge<T>* he = halfedge_new (internal, edge, direction);
            halfedge_link (leftleft, he);
            endpos (internal, edge, &vertex, DIRECTION_RIGHT - direction);

            point<T> p;
            if (check_circle_event (leftleft, he, &p)) {
                pq_remove (internal->eventqueue, leftleft);
                leftleft->vertex    = p;
                leftleft->y         = p[1] + point_dist (&bottom->p, &p);
                pq_push (internal->eventqueue, leftleft);
            }
            if (check_circle_event (he, rightright, &p)) {
                he->vertex      = p;
                he->y           = p[1] + point_dist (&bottom->p, &p);
                pq_push (internal->eventqueue, he);
            }
        }

        typedef union cast_align_struct_
        {
            char*   charp;
            void**  voidpp;
        } cast_align_struct;

        static void rect_union (rect<T>* rect, const point<T>* p)
        {
            rect->min[0] = std::min (rect->min[0], p->x());
            rect->min[1] = std::min (rect->min[1], p->y());
            rect->max[0] = std::max (rect->max[0], p->x());
            rect->max[1] = std::max (rect->max[1], p->y());
        }

        static void rect_round (rect<T>* rect)
        {
            rect->min[0] = std::floor (rect->min[0]);
            rect->min[1] = std::floor (rect->min[1]);
            rect->max[0] = std::ceil (rect->max[0]);
            rect->max[1] = std::ceil (rect->max[1]);
        }

        static void rect_inflate (rect<T>* rect, T amount)
        {
            rect->min[0] -= amount;
            rect->min[1] -= amount;
            rect->max[0] += amount;
            rect->max[1] += amount;
        }

        static int prune_duplicates (context_internal<T>* internal, rect<T>* _rect)
        {
            int num_sites = internal->numsites;
            site<T>* sites = internal->sites;

            rect<T> r;
            r.min[0] = r.min[1] = std::numeric_limits<T>::max();
            r.max[0] = r.max[1] = std::numeric_limits<T>::lowest();

            int offset = 0;
            // Prune duplicates first
            for (int i = 0; i < num_sites; i++) {
                const site<T>* s = &sites[i];
                // Remove duplicates, to avoid anomalies
                if (i > 0 && equal (&s->p, &sites[i - 1].p)) {
                    offset++;
                    continue;
                }

                sites[i - offset] = sites[i];

                rect_union (&r, &s->p);
            }
            internal->numsites -= offset;
            if (_rect) { *_rect = r; }
            return offset;
        }

        static int prune_not_in_shape (context_internal<T>* internal, rect<T>* _rect)
        {
            int num_sites = internal->numsites;
            site<T>* sites = internal->sites;

            rect<T> r;
            r.min[0] = r.min[1] = std::numeric_limits<T>::max();
            r.max[0] = r.max[1] = std::numeric_limits<T>::lowest();

            int offset = 0;
            for (int i = 0; i < num_sites; i++) {
                const site<T>* s = &sites[i];

                if (!internal->clipper_.test_fn (&internal->clipper_, s->p)) {
                    offset++;
                    continue;
                }

                sites[i - offset] = sites[i];

                rect_union (&r, &s->p);
            }
            internal->numsites -= offset;
            if (_rect) { *_rect = r; }
            return offset;
        }

        static context_internal<T>* alloc_internal (int num_points, void* userallocctx, FJCVAllocFn allocfn, FJCVFreeFn freefn)
        {
            // Interesting limits from Euler's equation
            // Slide 81: https://courses.cs.washington.edu/courses/csep521/01au/lectures/lecture10slides.pdf
            // Page 3: https://sites.cs.ucsb.edu/~suri/cs235/Voronoi.pdf
            size_t eventssize = (size_t)(num_points*2) * sizeof(void*); // beachline can have max 2*n-5 parabolas
            size_t sitessize = (size_t)num_points * sizeof(site<T>);
            size_t memsize = sizeof(priorityqueue) + eventssize + sitessize + sizeof(context_internal<T>) + 16u; // 16 bytes padding for alignment

            char* originalmem = (char*)allocfn (userallocctx, memsize);
            std::memset (originalmem, 0, memsize);

            // align memory
            char* mem = (char*)align (originalmem, sizeof(void*));

            context_internal<T>* internal = (context_internal<T>*)mem;
            mem += sizeof(context_internal<T>);
            internal->mem    = originalmem;
            internal->memctx = userallocctx;
            internal->alloc  = allocfn;
            internal->free   = freefn;

            mem = (char*)align (mem, sizeof(void*));
            internal->sites = (site<T>*) mem;
            mem += sitessize;

            mem = (char*)align (mem, sizeof(void*));
            internal->eventqueue = (priorityqueue*)mem;
            mem += sizeof(priorityqueue);
            assert (((uintptr_t)mem & (sizeof(void*)-1)) == 0);

            cast_align_struct tmp;
            tmp.charp = mem;
            internal->eventmem = tmp.voidpp;

            assert ((mem+eventssize) <= (originalmem+memsize));

            return internal;
        }

        // This version of diagram_generate allows the client to use a custom allocator
        static void diagram_generate_useralloc (int num_points, const point<T>* points,
                                                const rect<T>* _rect, const clipper<T>* _clipper,
                                                void* userallocctx, FJCVAllocFn allocfn, FJCVFreeFn freefn, diagram<T>* d)
        {
            if (d->internal) { diagram_free (d); }

            context_internal<T>* internal = alloc_internal (num_points, userallocctx, allocfn, freefn);

            internal->beachline_start = halfedge_new (internal, 0, 0);
            internal->beachline_end = halfedge_new (internal, 0, 0);

            internal->beachline_start->left     = 0;
            internal->beachline_start->right    = internal->beachline_end;
            internal->beachline_end->left       = internal->beachline_start;
            internal->beachline_end->right      = 0;

            internal->last_inserted = 0;

            int max_num_events = num_points * 2; // beachline can have max 2*n-5 parabolas
            pq_create (internal->eventqueue, max_num_events, (void**)internal->eventmem);

            internal->numsites = num_points;
            site<T>* sites = internal->sites;

            for (int i = 0; i < num_points; ++i) {
                sites[i].p        = points[i];
                sites[i].edges    = 0;
                sites[i].index    = i;
            }

            qsort (sites, (size_t)num_points, sizeof(site<T>), point_cmp);

            clipper<T> box_clipper;
            if (_clipper == nullptr) {
                box_clipper.test_fn = &jcv::manager<T>::boxshape_test;
                box_clipper.clip_fn = &jcv::manager<T>::boxshape_clip;
                box_clipper.fill_fn = &jcv::manager<T>::boxshape_fill;
                _clipper = &box_clipper;
            }
            internal->clipper_ = *_clipper;

            rect<T> tmp_rect;
            tmp_rect.min[0] = tmp_rect.min[1] = std::numeric_limits<T>::max();
            tmp_rect.max[0] = tmp_rect.max[1] = std::numeric_limits<T>::lowest();
            prune_duplicates (internal, &tmp_rect);

            // Prune using the test second
            if (internal->clipper_.test_fn) {
                // e.g. used by the box clipper in the test_fn
                internal->clipper_.min = _rect ? _rect->min : tmp_rect.min;
                internal->clipper_.max = _rect ? _rect->max : tmp_rect.max;

                prune_not_in_shape (internal, &tmp_rect);

                // The pruning might have made the bounding box smaller
                if (!_rect) {
                    // In the case of all sites being all on a horizontal or vertical line, the
                    // rect area will be zero, and the diagram generation will most likely fail
                    rect_round(&tmp_rect);
                    rect_inflate(&tmp_rect, 10);

                    internal->clipper_.min = tmp_rect.min;
                    internal->clipper_.max = tmp_rect.max;
                }
            }

            internal->rect_ = _rect ? *_rect : tmp_rect;

            d->min      = internal->rect_.min;
            d->max      = internal->rect_.max;
            d->numsites = internal->numsites;
            d->internal = internal;

            internal->bottomsite = nextsite (internal);

            priorityqueue* pq = internal->eventqueue;
            site<T>* site = nextsite (internal);

            int finished = 0;
            while (!finished) {

                point<T> lowest_pq_point;
                if (!pq_empty (pq)) {
                    halfedge<T>* he = (halfedge<T>*)pq_top (pq);
                    lowest_pq_point[0] = he->vertex[0];
                    lowest_pq_point[1] = he->y;
                }

                if (site != 0 && (pq_empty(pq) || lessthan (&site->p, &lowest_pq_point))) {
                    site_event (internal, site);
                    site = nextsite (internal);
                } else if (!pq_empty (pq)) {
                    circle_event (internal);
                } else {
                    finished = 1;
                }
            }

            for (halfedge<T>* he = internal->beachline_start->right; he != internal->beachline_end; he = he->right) {
                finishline (internal, he->edge_);
            }

            fillgaps (d);
        }

        /**
         * Uses malloc
         * If a clipper is not supplied, a default box clipper will be used
         * If rect is null, an automatic bounding box is calculated, with an extra padding of 10 units
         * All points will be culled against the bounding rect, and all edges will be clipped against it.
         */
        static void diagram_generate (int num_points, const point<T>* points, const rect<T>* rect, const clipper<T>* clipper, diagram<T>* d)
        {
            diagram_generate_useralloc (num_points, points, rect, clipper, 0, alloc_fn, free_fn, d);
        }

        static void diagram_generate (int num_points, const point<T>* points, const clipper<T>* clipper, diagram<T>* d)
        {
            diagram_generate_useralloc (num_points, points, 0,    clipper, 0, alloc_fn, free_fn, d);
        }

        // User API
        void diagram_generate (const std::vector<point<T>>& centres)
        {
            int ncoords = static_cast<int>(centres.size());
            sm::interval<T> rx, ry;
            rx.search_init();
            ry.search_init();
            for (int i = 0; i < ncoords ; ++i) {
                rx.update (centres[i][0]);
                ry.update (centres[i][1]);
            }
            std::memset (&this->diagram, 0, sizeof(jcv::diagram<T>));
            this->domain = {
                jcv::point<T>{rx.min - this->border_width, ry.min - this->border_width, 0.0f},
                jcv::point<T>{rx.max + this->border_width, ry.max + this->border_width, 0.0f}
            };
            jcv::manager<T>::diagram_generate (ncoords, centres.data(), &this->domain, 0, &this->diagram);
        }

        // User API to generate with a polygon boundary
        void diagram_generate (const std::vector<point<T>>& centres, std::vector<point<T>>& polygon)
        {
            int ncoords = static_cast<int>(centres.size());

            std::memset (&this->diagram, 0, sizeof(jcv::diagram<T>));

            jcv::clipper<T> polygonclipper;
            polygonclipper.test_fn = &jcv::manager<T>::polygon_test;
            polygonclipper.clip_fn = &jcv::manager<T>::polygon_clip;
            polygonclipper.fill_fn = &jcv::manager<T>::polygon_fill;
            polygonclipper.ctx = &polygon;

            jcv::manager<T>::diagram_generate (ncoords, centres.data(), &polygonclipper, &this->diagram);
        }

        int diagram_numsites() const { return this->diagram.numsites; }

        static int polygon_test (const clipper<T>* clipper, const point<T> p)
        {
            auto polygon = reinterpret_cast<std::vector<jcv::point<T>>*>(clipper->ctx);
            int num_points = polygon->size();

            // convex polygon
            // winding CCW
            // all polygon normals point outward
            // if the point is in front of the plane, it is outside

            int result = 1;
            for (int i = 0; i < num_points; ++i) {
                point<T> p0 = (*polygon)[i];
                point<T> p1 = (*polygon)[(i + 1) % num_points];
                point<T> n = {};
                n[0] = p1.y() - p0.y();
                n[1] = p0.x() - p1.x();
                point<T> diff = p - p0;

                if (n.dot (diff) > 0) {
                    result = 0;
                    break;
                }
            }
            return result;
        }

        static int ray_intersect_polygon (const clipper<T>* clipper, point<T>& p0, point<T>& p1)
        {
            constexpr bool debug_ray_intersect = false;
            auto polygon = reinterpret_cast<std::vector<jcv::point<T>>*>(clipper->ctx);
            int num_points = polygon->size();

            // First wind to find out if p0 or p1 is inside clipper's boundary
            sm::winder w (*polygon); // winder ignores z
            int w_p0 = w.wind (p0);
            int w_p1 = w.wind (p1);

            if (w_p0 == 0 && w_p1 == 0) {
                if constexpr (debug_ray_intersect) { std::cout << "Both points outside boundary\n"; }
                return 2; // Both outside means remove this edge.
            } else if (w_p0 != 0 && w_p1 != 0) {
                if constexpr (debug_ray_intersect) { std::cout << "Both points INSIDE boundary, return 1\n"; }
                return 1; // Both inside
            }

            if constexpr (debug_ray_intersect) {
                std::cout << "p0 is " << (w_p0 == 0 ? "outside" : "inside") << " and p1 is "
                          <<  (w_p1 == 0 ? "outside" : "inside") << std::endl;
            }
            for (int i = 0; i < num_points; ++i) {
                sm::vec<T, 2> v0 = (*polygon)[i].less_one_dim();
                sm::vec<T, 2> v1 = (*polygon)[(i + 1) % num_points].less_one_dim();

                if constexpr (debug_ray_intersect) {
                    std::cout << "Test segments intersect for v0/v1: " << v0 << ", " << v1 << ", p0/p1: "
                              << p0.less_one_dim() << ", " << p1.less_one_dim() << std::endl;
                }

                // find crossing point of v0,v1 and p0,p1
                std::bitset<2> isect = sm::geometry::segments_intersect (v0, v1, p0.less_one_dim(), p1.less_one_dim());
                if constexpr (debug_ray_intersect) {
                    std::cout << "Intersect? " << (isect.test(0) ? "yes" : "no")
                              << " colinear? " << (isect.test(1) ? "yes" : "no") << std::endl;
                }
                if (isect.test(0) == true) {
                    if (isect.test(1) == true) {
                        // lines co-linear. This is always an error?
                        return 0;
                    }
                    // lines intersect. Find intersection point
                    sm::vec<T, 2> cp = sm::geometry::crossing_point (v0, v1, p0.less_one_dim(), p1.less_one_dim());

                    if (w_p0 != 0) { // p0 inside, p1 outside
                        p1[0] = cp[0];
                        p1[1] = cp[1];
                    } else if (w_p1 != 0) {
                        p0[0] = cp[0];
                        p0[1] = cp[1];
                    } else {
                        // Neither p0 nor p1 were inside the polygon
                        return 0;
                    }

                } else {
                    if (isect.test(1) == true) {
                        // lines co-linear. This is always an error?
                        return 0;
                    } // else no crossing point with this section.
                }
            }

            return 1;
        }

        static int polygon_clip (const clipper<T>* clipper, edge<T>* e)
        {
            constexpr bool debug_polyclip = false;

            // Using the box clipper to get a finite line segment
            int result = manager<T>::boxshape_clip (clipper, e);
            if (!result) { return 0; }

            // Return here for a sanity check on the polygon clipping
            //return 1;

            point<T> p0 = e->pos[0];
            point<T> p1 = e->pos[1];

            if constexpr (debug_polyclip) {
                std::cout << "**Clip for: " << p0 << " to " << p1 << std::endl;
            }

            result = ray_intersect_polygon (clipper, p0, p1);

            if (result == 2) {
                // Both p0 and p1 were outside boundary.
                return result;
            } else if (result == 0) {
                e->pos[0] = e->pos[1];
                return result;
            } // else result should be 1, which is ok

            e->pos[0] = p0;
            e->pos[1] = p1;

            if constexpr (debug_polyclip) {
                std::cout << "Clipped to: " << p0 << " to " << p1 << std::endl;
            }

            return 1;
        }

        // Get the polygon vertex with index vtx_idx from the clipper
        static point<T> get_polygon_vertex (const clipper<T>* clipper, int vtx_idx)
        {
            point<T> p = {};
            auto polygon = reinterpret_cast<std::vector<jcv::point<T>>*>(clipper->ctx);
            int num_points = polygon->size();
            if (vtx_idx < num_points) { p = (*polygon)[vtx_idx]; }
            return p;
        }

        // Find which edge (or vertex) of the polygon in the clipper a point _p lies on
        //
        // In field 0, return: 0: p NOT on polygon; 1: p on polygon edge section; 2: p on polygon vertex
        // In field 1, return the index of the edge or vertex referred to in field 0.
        static sm::vec<int, 2> find_polygon_edge (const clipper<T>* clipper, const point<T>& _p)
        {
            point<T> p = _p;
            sm::vec<int, 2> info = {};
            if (std::isnan(p[2])) { p[2] = T{0}; }

            auto polygon = reinterpret_cast<std::vector<jcv::point<T>>*>(clipper->ctx);
            T min_dist = std::numeric_limits<T>::max();
            int num_points = polygon->size();

            for (int i = 0; i < num_points; ++i) {

                point<T> p0 = (*polygon)[i];
                if (p == p0) {
                    // ON vertex p0
                    info[0] = 2;
                    info[1] = i;
                    break;
                }

                point<T> p1 = (*polygon)[(i + 1) % num_points];
                if (p == p1) {
                    // ON vertex p1
                    info[0] = 2;
                    info[1] = (i + 1) % num_points;
                    break;
                }

                // Now is p on the edge?
                point<T> vsegment = p1 - p0;
                point<T> vpoint = p - p0;
                T t = vsegment.dot (vpoint) / vsegment.dot (vsegment);
                if (t < T{0} || t > T{1}) { continue; }
                point<T> projected = p0 + vsegment * t; // projection of vpoint onto vsegment
                T distsq = (p - projected).length_sq();

                if (distsq < min_dist) {
                    min_dist = distsq;
                    info[0] = 1;
                    info[1] = i;
                }
            }

            return info;
        }

        static void polygon_fill (const clipper<T>* clipper,
                                  context_internal<T>* allocator, site<T>* site)
        {
            constexpr bool debug_polyfill = false;

            // They're sorted CCW, so if the current->pos[1] != next->pos[0], then we have a gap
            auto polygon = reinterpret_cast<std::vector<jcv::point<T>>*>(clipper->ctx);
            int num_points = polygon->size();

            graphedge<T>* curr_graphedge = site->edges;
            if (!curr_graphedge) {
                graphedge<T>* gap = alloc_graphedge (allocator);
                gap->neighbor = 0;
                // Pick the first edge of the polygon (which is also CCW)
                gap->pos[0] = (*polygon)[0];
                gap->pos[1] = (*polygon)[1];
                gap->angle  = calc_sort_metric (site, gap);
                gap->next   = 0;
                gap->edge_  = create_gap_edge (allocator, site, gap);

                curr_graphedge = gap;
                site->edges = gap;
            }

            graphedge<T>* next = curr_graphedge->next;
            if (!next) {
                graphedge<T>* gap = alloc_graphedge (allocator);

                sm::vec<int, 2> polygon_info = find_polygon_edge (clipper, curr_graphedge->pos[1]);
                int polygon_edge = polygon_info[1];
                if (!(curr_graphedge->pos[1] == (*polygon)[(polygon_edge + 1) % num_points])) {
                    gap->pos[0] = curr_graphedge->pos[1];
                    gap->pos[1] = (*polygon)[(polygon_edge + 1) % num_points];
                } else {
                    gap->pos[0] = (*polygon)[(polygon_edge + 1) % num_points];
                    gap->pos[1] = (*polygon)[(polygon_edge + 2) % num_points];
                }

                gap->neighbor   = 0;
                gap->angle      = calc_sort_metric (site, gap);
                gap->next       = 0;
                gap->edge_      = create_gap_edge (allocator, site, gap);

                gap->next = curr_graphedge->next;
                curr_graphedge->next = gap;
                curr_graphedge = gap;
                next = site->edges;
            }

            constexpr int loopcount_thresh = 1024;
            int loopcount = 0;
            while (curr_graphedge && next) {

                // Which edge of the polygon, if any, are we on? current_edge[0] indicates whether
                // the point is "not edge/vertex" (value 0); "on an edge" (value 1) or "is a vertex"
                // (value 2). current_edge[1] indicates the index of the edge.
                sm::vec<int, 2> current_edge = find_polygon_edge (clipper, curr_graphedge->pos[1]);

                if (current_edge[0] > 0 && !equal (&curr_graphedge->pos[1], &next->pos[0])) {

                    sm::vec<int, 2> next_edge = find_polygon_edge (clipper, next->pos[0]);

                    if (current_edge[0] == 1 && next_edge[0] == 1 && current_edge[1] == next_edge[1]) {

                        // Case: Current and Next on the same border
                        if constexpr (debug_polyfill) { std::cout << "Current and next on same border\n"; }

                        graphedge<T>* gap = alloc_graphedge (allocator);
                        gap->neighbor   = 0;
                        gap->pos[0]     = curr_graphedge->pos[1];
                        gap->pos[1]     = next->pos[0];
                        gap->angle      = calc_sort_metric (site, gap);
                        gap->edge_      = create_gap_edge (allocator, site, gap);

                        gap->next = curr_graphedge->next;
                        curr_graphedge->next = gap;

                    } else if (current_edge[0] == 1 && next_edge[0] == 1 && next_edge[1] != current_edge[1]) {

                        // Case: Current and Next on different borders, so we need to find the
                        // adjacent vertex, following the borders CCW
                        if constexpr (debug_polyfill) { std::cout << "Current and next on different borders\n"; }

                        int next_vertex = (current_edge[1] + 1) % num_points;
                        point<T> vtx = get_polygon_vertex (clipper, next_vertex);
                        if constexpr (debug_polyfill) { std::cout << "vtx = " << vtx << std::endl; }

                        graphedge<T>* gap = alloc_graphedge (allocator);
                        gap->neighbor   = 0;
                        gap->pos[0]     = curr_graphedge->pos[1];
                        gap->pos[1]     = vtx;
                        gap->angle      = calc_sort_metric (site, gap);
                        gap->edge_      = create_gap_edge (allocator, site, gap);

                        gap->next = curr_graphedge->next;
                        curr_graphedge->next = gap;

                    } else if (current_edge[0] == 1 && next_edge[0] == 2) {

                        // Case: Current on border, next on a vertex
                        if constexpr (debug_polyfill) { std::cout << "Current on border next on vertex\n"; }
                        point<T> vtx = get_polygon_vertex (clipper, next_edge[1]);

                        graphedge<T>* gap = alloc_graphedge (allocator);
                        gap->neighbor   = 0;
                        gap->pos[0]     = curr_graphedge->pos[1];
                        gap->pos[1]     = vtx;
                        gap->angle      = calc_sort_metric (site, gap);
                        gap->edge_      = create_gap_edge (allocator, site, gap);

                        gap->next = curr_graphedge->next;
                        curr_graphedge->next = gap;

                    } else if (current_edge[0] == 2 && next_edge[0] == 1 && next_edge[1] == current_edge[1]) {

                        if constexpr (debug_polyfill) { std::cout << "Current on vertex next on same border\n"; }
                        // Case: Current on vertex, next on *same* border
                        graphedge<T>* gap = alloc_graphedge (allocator);
                        gap->neighbor   = 0;
                        gap->pos[0]     = curr_graphedge->pos[1];
                        gap->pos[1]     = next->pos[0];
                        gap->angle      = calc_sort_metric (site, gap);
                        gap->edge_      = create_gap_edge (allocator, site, gap);

                        gap->next = curr_graphedge->next;
                        curr_graphedge->next = gap;

                    } else if (current_edge[0] == 2 && next_edge[0] == 1 && next_edge[1] != current_edge[1]) {

                        if constexpr (debug_polyfill) { std::cout << "Current on vertex next on another border\n"; }
                        // Case: Current on vertex, next on another border, so we need to find the adjacent
                        // vertex, following the borders CCW
                        int next_vertex = (current_edge[1] + 1) % num_points;
                        point<T> vtx = get_polygon_vertex (clipper, next_vertex);

                        graphedge<T>* gap = alloc_graphedge (allocator);
                        gap->neighbor   = 0;
                        gap->pos[0]     = curr_graphedge->pos[1];
                        gap->pos[1]     = vtx;
                        gap->angle      = calc_sort_metric (site, gap);
                        gap->edge_      = create_gap_edge (allocator, site, gap);

                        gap->next = curr_graphedge->next;
                        curr_graphedge->next = gap;

                    } else if (next_edge[0] == 0) {

                        // Case: Current on vertex or border, but next not on polygon boundary
                        if constexpr (debug_polyfill) { std::cout << "Current on vertex or border, but next not on polygon boundary\n"; }
                        graphedge<T>* gap = alloc_graphedge (allocator);
                        gap->neighbor   = 0;
                        gap->pos[0]     = curr_graphedge->pos[1];
                        gap->pos[1]     = next->pos[0];
                        gap->angle      = calc_sort_metric (site, gap);
                        gap->edge_      = create_gap_edge (allocator, site, gap);

                        gap->next = curr_graphedge->next;
                        curr_graphedge->next = gap;

                    } else {
                        // next not on polygon?
                        std::cout << "Whoop whoop, unhandled case: current_edge = "
                                  << current_edge << " and next_edge = " << next_edge << "\n";
                    }
                } // else current_edge->pos[1] is not on the polygonal boundary

                if constexpr (debug_polyfill) {
                    if ((curr_graphedge->pos[0] - curr_graphedge->pos[1]).length_sq() > 25) {
                        std::cout << "added a long edge from " << curr_graphedge->pos[0] << " to " << curr_graphedge->pos[1] << std::endl;
                    }
                }

                curr_graphedge = curr_graphedge->next;
                if (curr_graphedge) {
                    next = curr_graphedge->next;
                    if (!next) { next = site->edges; }
                }
                ++loopcount;
                if (loopcount >= loopcount_thresh) { throw std::runtime_error ("Kaboom (too many loops)"); }

            }
        }

        /**
         * End of boundary clipping code
         */

        // User-configurable border width
        T border_width = std::numeric_limits<T>::epsilon();

    private:
        // Our diagram
        jcv::diagram<T> diagram;
        // A domain for the diagram.
        jcv::rect<T> domain = {};
    }; // end struct jcv::manager

} // namespace

/*
 * The Original, pre-mathplot about message follows:
 */

/*

ABOUT:

    A fast single file 2D voronoi diagram generator

HISTORY:
    0.9     2023-01-22  - Modified the Delaunay iterator creation api
    0.8     2022-12-20  - Added fix for missing border edges
                          More robust removal of duplicate graph edges
                          Added iterator for Delaunay edges
    0.7     2019-10-25  - Added support for clipping against convex polygons
                        - Added JCV_EDGE_INTERSECT_THRESHOLD for edge intersections
                        - Fixed issue where the bounds calculation wasn’t considering all points
    0.6     2018-10-21  - Removed JCV_CEIL/JCV_FLOOR/JCV_FABS
                        - Optimizations: Fewer indirections, better beach head approximation
    0.5     2018-10-14  - Fixed issue where the graph edge had the wrong edge assigned (issue #28)
                        - Fixed issue where a point was falsely passing the jcv_is_valid() test (issue #22)
                        - Fixed jcv_diagram_get_edges() so it now returns _all_ edges (issue #28)
                        - Added jcv_diagram_get_next_edge() to skip zero length edges (issue #10)
                        - Added defines JCV_CEIL/JCV_FLOOR/JCV_FLT_MAX for easier configuration
    0.4     2017-06-03  - Increased the max number of events that are preallocated
    0.3     2017-04-16  - Added clipping box as input argument (Automatically calculated if needed)
                        - Input points are pruned based on bounding box
    0.2     2016-12-30  - Fixed issue of edges not being closed properly
                        - Fixed issue when having many events
                        - Fixed edge sorting
                        - Code cleanup
    0.1                 Initial version

LICENSE:

    The MIT License (MIT)

    Copyright (c) 2015-2019 Mathias Westerdahl

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.


DISCLAIMER:

    This software is supplied "AS IS" without any warranties and support

USAGE:

    The input points are pruned if

        * There are duplicates points
        * The input points are outside of the bounding box (i.e. fail the clipping test function)
        * The input points are rejected by the clipper's test function

    The input bounding box is optional (calculated automatically)

    The input domain is (-FLT_MAX, FLT_MAX] (for floats)

    The api consists of these functions:

    void jcv_diagram_generate( int num_points, const jcv_point* points, const jcv_rect* rect, const jcv_clipper* clipper, jcv_diagram* diagram );
    void jcv_diagram_generate_useralloc( int num_points, const jcv_point* points, const jcv_rect* rect, const jcv_clipper* clipper, const jcv_clipper* clipper, void* userallocctx, FJCVAllocFn allocfn, FJCVFreeFn freefn, jcv_diagram* diagram );
    void jcv_diagram_free( jcv_diagram* diagram );

    const jcv_site* jcv_diagram_get_sites( const jcv_diagram* diagram );
    const jcv_edge* jcv_diagram_get_edges( const jcv_diagram* diagram );
    const jcv_edge* jcv_diagram_get_next_edge( const jcv_edge* edge );

    An example usage:

    #define JC_VORONOI_IMPLEMENTATION
    // If you wish to use doubles
    //#define JCV_REAL_TYPE double
    //#define JCV_ATAN2 atan2
    //#define JCV_FLT_MAX 1.7976931348623157E+308
    #include "jc_voronoi.h"

    void draw_edges(const jcv_diagram* diagram);
    void draw_cells(const jcv_diagram* diagram);

    void generate_and_draw(int numpoints, const jcv_point* points)
    {
        jcv_diagram diagram;
        memset(&diagram, 0, sizeof(jcv_diagram));
        jcv_diagram_generate(count, points, 0, 0, &diagram);

        draw_edges(diagram);
        draw_cells(diagram);

        jcv_diagram_free( &diagram );
    }

    void draw_edges(const jcv_diagram* diagram)
    {
        // If all you need are the edges
        const jcv_edge* edge = jcv_diagram_get_edges( diagram );
        while( edge )
        {
            draw_line(edge->pos[0], edge->pos[1]);
            edge = jcv_diagram_get_next_edge(edge);
        }
    }

    void draw_cells(const jcv_diagram* diagram)
    {
        // If you want to draw triangles, or relax the diagram,
        // you can iterate over the sites and get all edges easily
        const jcv_site* sites = jcv_diagram_get_sites( diagram );
        for( int i = 0; i < diagram->numsites; ++i )
        {
            const jcv_site* site = &sites[i];

            const jcv_graphedge* e = site->edges;
            while( e )
            {
                draw_triangle( site->p, e->pos[0], e->pos[1]);
                e = e->next;
            }
        }
    }

    // Here is a simple example of how to do the relaxations of the cells
    void relax_points(const jcv_diagram* diagram, jcv_point* points)
    {
        const jcv_site* sites = jcv_diagram_get_sites(diagram);
        for( int i = 0; i < diagram->numsites; ++i )
        {
            const jcv_site* site = &sites[i];
            jcv_point sum = site->p;
            int count = 1;

            const jcv_graphedge* edge = site->edges;

            while( edge )
            {
                sum[0] += edge->pos[0][0];
                sum[1] += edge->pos[0][1];
                ++count;
                edge = edge->next;
            }

            points[site->index][0] = sum[0] / count;
            points[site->index][1] = sum[1] / count;
        }
    }

 */
