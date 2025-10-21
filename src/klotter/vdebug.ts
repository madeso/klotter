type Color = string;

// API and ui partly inspired by https://github.com/SebLague/Visual-Debug

// todo(Gustav): generate self contained file with packed js file
// todo(Gustav): replace existing dump tool
// todo(Gustav): more rendering primitives
// todo(Gustav): improve npm build, minimize and optimize?
// todo(Gustav): add linter to npm build
// todo(Gustav): better colors
// todo(Gustav): expand with 3d option

interface SceneArtist
{
    showWhenInBackground: boolean;
    activeDrawColor: Color;
    inactiveDrawColor: Color;
}

const color_from_artist = (artist: SceneArtist, isActive: boolean) : Color =>
{
    return isActive ? artist.activeDrawColor : artist.inactiveDrawColor;
};

interface vec2 {
    x: number;
    y: number;
}
const find_closest = <T>(points: T[], d2: (p: T, index: number)=>number): [T|null, number, number] => {
    let closest = null;
    let smallest = 0;
    let found_index = -1;
    let index = 0;
    for(const src of points) {
        const current_index = index; index += 1;
        const d = d2(src, current_index);
        if(!closest || smallest > d) {
            closest = src;
            smallest = d;
            found_index = current_index;
        }
    }
    return [closest, smallest, found_index]
};

// min x, min y, max x, max y
interface AABB
{
    min_x: number;
    min_y: number;
    max_x: number;
    max_y: number
}

const include_point = (aabb: AABB|null, x: number, y: number): AABB =>
{
    if(!aabb) return {min_x: x, min_y: y, max_x: x, max_y: y};

    aabb.min_x = Math.min(aabb.min_x, x);
    aabb.min_y = Math.min(aabb.min_y, y);
    aabb.max_x = Math.max(aabb.max_x, x);
    aabb.max_y = Math.max(aabb.max_y, y);
    return aabb;
};

interface Frame<Artist>
{
    description: string;
    artists: Artist[];
    keepInBackground: boolean;
}

interface Camera
{
    pan_x: number;
    pan_y: number;
    scale: number;
}

const camera_from_aabb = (bb: AABB, space: number, canvas: [number, number]): Camera =>
{
    const [width, height] = canvas;
    const [size_x, size_y] = [
        bb.max_x - bb.min_x,
        bb.max_y - bb.min_y
    ];

    const suggested_scale: [number, number] = [(width-space*2) / size_x, (height-space*2) / size_y]
    const scale = Math.min(suggested_scale[0], suggested_scale[1]);

    const scaled: [number, number] = [
        size_x * scale,
        size_y * scale
    ]

    const offset_x = (width - scaled[0])/2;
    const offset_y = (height - scaled[1])/2;

    return {
        pan_x: offset_x - bb.min_x * scale,
        pan_y: offset_y - bb.min_y * scale,
        scale
    };
};

const for_each_artist = <Artist extends SceneArtist>(
    selected_frame_index: number,
    frames: Frame<Artist>[],
    draw_artist: (artist: Artist, is_current_frame: boolean)=>void
) => {
    for(let i=0; i<=selected_frame_index; i+=1) {
        const frame = frames[i];
        if(!frame) continue;
        
        const is_current_frame = selected_frame_index == i;
        const showFrame = is_current_frame || (frame.keepInBackground && selected_frame_index > i);
        if (!showFrame) return;
        for (const artist of frame.artists)
        {
            if (is_current_frame || artist.showWhenInBackground)
            {
                draw_artist(artist, is_current_frame);
            }
        }
    }
};

interface PlotInterface {
    "type": "plot";
    start: number;
    end: number;
    step: number;
    values: number[]
}

type Artist2 =
      SceneArtist & {"type": "rect", x: number, y: number, w: number, h: number}
    | SceneArtist & {"type": "point", x: number, y: number, radius: number}
    | SceneArtist & {"type": "fillpoint", x: number, y: number, radius: number}
    | SceneArtist & {"type": "text", x: number, y: number, text: string}
    | SceneArtist & {"type": "line", points: {x: number, y: number}[]}
    | SceneArtist & {"type": "arrow", x1: number, y1: number, x2: number, y2: number, size: number}
    | SceneArtist & PlotInterface
;

const foreach_plot_point = (plot: PlotInterface, callback: (x: number, y: number)=>void) => {
    let x = plot.start;
    for(const y of plot.values) {
        callback(x, y);
        x += plot.step;
    }
};

const canvas_arrow = (context: CanvasRenderingContext2D, fromx: number, fromy: number, tox: number, toy: number, headlen: number) => {
  const dx = tox - fromx;
  const dy = toy - fromy;
  const angle = Math.atan2(dy, dx);
  context.beginPath();
  context.moveTo(fromx, fromy);
  context.lineTo(tox, toy);
  context.lineTo(tox - headlen * Math.cos(angle - Math.PI / 6), toy - headlen * Math.sin(angle - Math.PI / 6));
  context.moveTo(tox, toy);
  context.lineTo(tox - headlen * Math.cos(angle + Math.PI / 6), toy - headlen * Math.sin(angle + Math.PI / 6));
  context.stroke();
}

interface DebugData {
    frames: Frame<Artist2>[];
    y_positive: boolean;
}

(window as any).register_vdebug2 = (
    ui: {canvas: HTMLCanvasElement,
        focus: HTMLButtonElement,
        frame: HTMLInputElement,
        first_frame: HTMLButtonElement,
        previous_frame: HTMLButtonElement,
        next_frame: HTMLButtonElement,
        last_frame: HTMLButtonElement,
        description: HTMLSpanElement,
        status: HTMLSpanElement,
        hover: HTMLParagraphElement},
    data: DebugData) => {
    const additional_space = 10;

    const x_scale = 1;
    const y_scale = data.y_positive ? -1 : 1;

    const ctx = ui.canvas.getContext("2d");
    if(!ctx) {
        console.error("Missing 2d support");
        return;
    }
    const fetch_current_frame_index = () => {
        const value = ui.frame.valueAsNumber;
        const currentFrame = Number.isNaN(value) ? 0 : value;
        return currentFrame;
    }
    // todo(Gustav): switch to constref camera struct
    let pan_x = 0;
    let pan_y = 0;
    let scale = 1.0;

    const set_cam_from_aabb = (aabb: AABB | undefined) => {
        if(!aabb) return;
        const cam = camera_from_aabb(aabb, additional_space, [ui.canvas.width, ui.canvas.height]);
        pan_x = cam.pan_x;
        pan_y = cam.pan_y;
        scale = cam.scale;
    };

    const calc_aabb = (frame_index: number): AABB => {
        let aabb : AABB | null = null;
        for_each_artist(frame_index, data.frames, (artist) => {
            switch(artist.type) {
                case "arrow":
                    aabb = include_point(aabb, artist.x1 * x_scale, artist.y1 * y_scale);
                    aabb = include_point(aabb, artist.x2 * x_scale, artist.y2 * y_scale);
                    break;
                case "line":
                    for(const p of artist.points) {
                        aabb = include_point(aabb, p.x * x_scale, p.y * y_scale);
                    }
                    break;
                case "fillpoint":
                case "point":
                    aabb = include_point(aabb, artist.x * x_scale, artist.y * y_scale);
                    break;
                case "rect":
                    aabb = include_point(aabb, artist.x * x_scale, artist.y * y_scale);
                    aabb = include_point(aabb, (artist.x + artist.w) * x_scale, (artist.y + artist.h) * y_scale);
                    break;
                case "text":
                    // todo(Gustav): include width somehow...
                    aabb = include_point(aabb, artist.x * x_scale, artist.y * y_scale);
                    break;
                case "plot":
                    foreach_plot_point(artist, (x, y) => {
                        aabb = include_point(aabb, x * x_scale, y * y_scale);
                    });
                    break;
            }
        });

        return aabb ?? {min_x: 0, min_y: 0, max_x: 0, max_y: 0};
    };

    // set initial pan
    set_cam_from_aabb(calc_aabb(0));

    // world to screen
    const px = (x: number) => pan_x + x * scale * x_scale;
    const py = (y: number) => pan_y + y * scale * y_scale;
    const ps = (s: number) => s * scale;
    // screen 2 world
    const rpx = (x: number) => (x-pan_x) / (scale * x_scale);
    const rpy = (y: number) => (y-pan_y) / (scale * y_scale);

    type HoverInfo = { html: string, type: Artist2['type'], hover:
        null
        | {type: "point", x: number, y: number, radius: number}
        | {type: "plot", x: number, y: number, min: number, max: number}
    };
    let current_hover_info: HoverInfo[] | null = null;

    const lerp = (from: number, frac: number, to: number) => {
        return from + frac * (to - from);
    }

    const find_hover_candidate = (plot: SceneArtist & PlotInterface, mouse: vec2): HoverInfo|null => {
        const x = rpx(mouse.x);
        const min_y_screen = Math.min(...plot.values);
        const max_y_screen = Math.max(...plot.values);
        const min_y = py(min_y_screen);
        const max_y = py(max_y_screen);
        if(x < plot.start) return null;
        if(x > plot.end) return null;
        if(mouse.y < max_y) return null;
        if(mouse.y > min_y) return null;

        
        const base = x/plot.step;
        const index = Math.floor(base);
        const frac = base - index;
        const lhs = plot.values[index];
        const rhs = plot.values[index+1];
        if(lhs === undefined) return null;
        const y = rhs ? lerp(lhs, frac, rhs) : lhs;

        return {
            html: '<span style="color:' + plot.activeDrawColor +'">(' + y + ')</span>',
            type: plot.type,
            hover: {
                type: "plot",
                x: x,
                y: y,
                min: min_y_screen,
                max: max_y_screen
            }
        };
    };

    const hover_color = "#f00";

    const draw = () => {
        ctx.save();
        ctx.fillStyle = "lightgray";
        ctx.fillRect(0, 0, ui.canvas.width, ui.canvas.height);
        const selected_frame_index = fetch_current_frame_index();
        for_each_artist(selected_frame_index, data.frames, (a, is_current_frame) => {
            const color = color_from_artist(a, is_current_frame);
            switch(a.type) {
            case "line":
                ctx.strokeStyle = color;
                ctx.beginPath();

                {let first = true;
                for(const p of a.points) {
                    const is_first = first;
                    if(first) first = false;
                    if(is_first) ctx.moveTo(px(p.x), py(p.y));
                    else ctx.lineTo(px(p.x), py(p.y));
                }}
                ctx.stroke();
                break;
            case "arrow":
                ctx.strokeStyle = color;
                canvas_arrow(ctx, px(a.x1), py(a.y1), px(a.x2), py(a.y2), a.size);
                break;
            case "point":
                ctx.strokeStyle = color;
                ctx.beginPath();
                ctx.arc(px(a.x), py(a.y), a.radius, 0, 2 * Math.PI);
                ctx.stroke();
                break;
            case "text":
                ctx.fillText(a.text, px(a.x), py(a.y));
                break;
            case "fillpoint":
                ctx.fillStyle = color;
                ctx.beginPath();
                ctx.arc(px(a.x), py(a.y), a.radius, 0, 2 * Math.PI);
                ctx.fill();
                break;
            case "rect":
                ctx.fillStyle = color;
                ctx.fillRect(px(a.x), py(a.y), ps(a.w), ps(a.h));
                break;
            case "plot":
                ctx.strokeStyle = color;
                ctx.beginPath();
                {let first = true;
                foreach_plot_point(a, (x, y) => {
                    const is_first = first;
                    if(first) first = false;
                    if(is_first) ctx.moveTo(px(x), py(y));
                    else ctx.lineTo(px(x), py(y));
                })};
                ctx.stroke();
                break;
            }
        });

        if(current_hover_info == null) return;
        for(const hover of current_hover_info.map(x => x.hover)) {
            if(!hover) continue;
            switch(hover.type) {
            case "point":
                ctx.strokeStyle = hover_color;
                ctx.beginPath();
                ctx.arc(px(hover.x), py(hover.y), hover.radius, 0, 2 * Math.PI);
                ctx.stroke();
                break;
            case "plot":
                ctx.strokeStyle = hover_color;
                
                ctx.beginPath();
                ctx.moveTo(px(hover.x), py(hover.min));
                ctx.lineTo(px(hover.x), py(hover.max));
                
                ctx.moveTo(px(hover.x)-10, py(hover.y));
                ctx.lineTo(px(hover.x)+10, py(hover.y));
                ctx.stroke();

                // ctx.beginPath();
                // ctx.arc(px(hover.x), py(hover.y), 10, 0, 2 * Math.PI);
                // ctx.stroke();
                break;
            }
        }
    };
    const set_current_frame_and_redraw = (this_frame: number) => {
        let frame_index = this_frame;
        
        if(frame_index >= data.frames.length) {
            frame_index = data.frames.length - 1;
        }
        if(frame_index < 0 ) {
            frame_index = 0;
        }

        ui.frame.value = frame_index.toString();

        const frame = data.frames[frame_index];
        ui.status.innerHTML = `<b>${frame_index + 1}</b> of <b>${data.frames.length}</b>:`
        ui.description.innerText = frame?.description ?? "";

        draw();
    }

    const step_frame = (calc_next_frame:(frame: number)=> number) => {
        const current_frame = fetch_current_frame_index();
        const next_frame = calc_next_frame(current_frame);
        set_current_frame_and_redraw(next_frame);
    };
    const set_first_frame = () => {
        set_current_frame_and_redraw(0);
    };
    const set_last_frame = () => {
        set_current_frame_and_redraw(data.frames.length - 1);
    };

    ui.first_frame.addEventListener("click", () => {
        set_first_frame();
    });
    ui.previous_frame.addEventListener("click", () => {
        step_frame(current_frame => current_frame - 1);
    });
    ui.next_frame.addEventListener("click", () => {
        step_frame(current_frame => current_frame + 1);
    });
    ui.last_frame.addEventListener("click", () => {
        set_last_frame();
    });

    set_current_frame_and_redraw(fetch_current_frame_index());
    ui.frame.min = "0";
    ui.frame.max = (data.frames.length -1).toString();

    ui.frame.addEventListener("input", () => {
        set_current_frame_and_redraw(fetch_current_frame_index());
    });

    const collect_hover_info = (frame: number, ui_mouse: vec2): HoverInfo[] => {
        const lines = new Array<HoverInfo>();
        const include_hover = (candidate: HoverInfo) => {
            for(let line_index = 0; line_index<lines.length; line_index += 1) {
                const src = lines[line_index];
                console.assert(src !== undefined, src); if(!src) continue;
                if(src.html !== candidate.html) continue;
                // todo(Gustav): rank "lines"

                // remove the duplicate and continue
                lines.splice(line_index, 1);
                line_index -= 1;
            }
            // todo(Gustav): expand with artist name
            // keep in mind that we probably wangt to do a double take when hovering a plot widget in the near future
            lines.push(candidate);
        };
        const include_points = (from: Artist2['type'], points: {x: number, y: number}[]) => {
            const [closest, distance2] = find_closest(points, (p) => {
                const dx = px(p.x) - ui_mouse.x;
                const dy = py(p.y) - ui_mouse.y
                return dx*dx + dy*dy;
            });
            if(closest === null) return;
            const distance = Math.sqrt(distance2);
            if(distance > 10) return;

            const candidate: HoverInfo = {
                html: '<span>(' + closest.x + ' ' + closest.y + ')</span>',
                type: from,
                hover: {
                    type: "point",
                    x: closest.x,
                    y: closest.y,
                    radius: distance
                }
            };

            include_hover(candidate);
        };
        for_each_artist(frame, data.frames, a => {
            switch(a.type) {
            case "line":
                include_points(a.type, a.points);
                break;
            case "arrow":
                include_points(a.type, [{x: a.x1, y: a.y1}, {x: a.x2, y: a.y2}]);
                break;
            case "point":
                include_points(a.type, [a]);
                break;
            case "text":
                break;
            case "fillpoint":
                include_points(a.type, [a]);
                break;
            case "rect":
                // todo(Gustav): improve rect with all corners
                include_points(a.type, [a]);
                break;
            case "plot":
                {
                    const candidate = find_hover_candidate(a, ui_mouse);
                    if(candidate) {
                        include_hover(candidate);
                    }
                }
                break;
            }
        });
        return lines;
    }

    const drag_button = 0;
    let down : {client_x: number, client_y: number, pan_x: number, pan_y: number} | null = null;
    const display_hover = (ev: MouseEvent) => {
        const frame = fetch_current_frame_index();
        const hovers = collect_hover_info(frame, {x: ev.offsetX, y: ev.offsetY});
        current_hover_info = hovers;
        draw();

        ui.hover.style.display = 'block';
        let html = '' + rpx(ev.offsetX) + ' x ' + rpy(ev.offsetY);
		for(const hov of hovers) {
            html += '<br />' + hov.html;
        }
        ui.hover.innerHTML = html;
	    ui.hover.style.left = (ev.clientX + 10) + 'px';
        ui.hover.style.top = (ev.clientY + 10) + 'px';
    };
    const hide_hover = () => {
        ui.hover.style.display = 'none';
        current_hover_info = null;
        draw();
    }
    ui.canvas.addEventListener("mousedown", ev => {
        if(ev.button === drag_button) {
            down = {client_x: ev.clientX, client_y: ev.clientY, pan_x, pan_y};
            draw();
        }
    });
    window.addEventListener("mousemove", ev => {
        if(down) {
            pan_x = down.pan_x + (ev.clientX - down.client_x);
            pan_y = down.pan_y + (ev.clientY - down.client_y);
            draw();
        }
    });
    ui.canvas.addEventListener("mousemove", ev => {
        display_hover(ev);
    });
    window.addEventListener("mouseup", ev => {
        if(ev.button === drag_button) {
            down = null;
        }
    });
    ui.canvas.addEventListener("mouseenter", ev => {
        display_hover(ev);
    });
    ui.canvas.addEventListener("mouseleave", () => {
        draw();
        hide_hover();
    });
    ui.canvas.addEventListener("wheel", ev => {
        const [mouse_x, mouse_y] = [ev.offsetX, ev.offsetY];

        const focus_x = rpx(mouse_x);
        const focus_y = rpy(mouse_y);

        const zoom = -ev.deltaY / 20;
        const scale_factor = 1 + 0.01 * Math.abs(zoom);
        if(zoom < 0.0) { scale /= scale_factor; }
        if(zoom > 0.0) { scale *= scale_factor; }

        const new_focus_x = rpx(mouse_x);
        const new_focus_y = rpy(mouse_y);
        pan_x = pan_x + (new_focus_x - focus_x) * scale * x_scale;
        pan_y = pan_y + (new_focus_y - focus_y) * scale * y_scale;

        draw();
        display_hover(ev);
    });

    ui.canvas.addEventListener("dblclick", _ => {
        ui.canvas.requestFullscreen();
    });

    let old_width = 0;
    let old_height = 0;
    ui.canvas.addEventListener("fullscreenchange", _ => {
        const is_entering = !!document.fullscreenElement;
        if (is_entering) {
            old_width = ui.canvas.width;
            old_height = ui.canvas.height;
            ui.canvas.width = window.screen.width;
            ui.canvas.height = window.screen.height;
        }
        else {
            ui.canvas.width = old_width;
            ui.canvas.height = old_height;
        }

        set_cam_from_aabb(calc_aabb(fetch_current_frame_index()));
        draw();
    });

    const resize_canvas = () => {
        ui.canvas.width = window.innerWidth;
        ui.canvas.height = window.innerHeight;
        set_cam_from_aabb(calc_aabb(fetch_current_frame_index()));
        draw();
    };
    const foucus_current_frame = () => {
        set_cam_from_aabb(calc_aabb(fetch_current_frame_index()));
        draw();
    }
    ui.focus.addEventListener("click", () => {
        foucus_current_frame();
    });
    window.addEventListener("resize", resize_canvas);
    window.addEventListener("keydown", ev => {
        const ctrl = ev.ctrlKey || ev.altKey;
        const shift = ev.shiftKey;
        const key = ev.key.toLowerCase();
        const get_step = () => {
            if(shift && ctrl) return 10;
            if(shift) return 5;
            if(ctrl) return 5;
            return 1;
        };
        if(key === "arrowleft") {
            step_frame(current_frame => current_frame - get_step());
        }
        else if (key === "arrowright") {
            step_frame(current_frame => current_frame + get_step());
        }
        else if(key === "f") {
            foucus_current_frame();
        }
        else if(key == "home") {
            set_first_frame();
        }
        else if(key == "end") {
            set_last_frame();
        }
        else {
            console.log(key);
        }
    })
    resize_canvas();
}
