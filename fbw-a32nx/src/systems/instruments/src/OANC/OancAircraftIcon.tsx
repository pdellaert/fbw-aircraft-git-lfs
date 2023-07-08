import { DisplayComponent, FSComponent, MappedSubject, Subscribable, VNode } from '@microsoft/msfs-sdk';

export interface OancAircraftIconProps {
    x: Subscribable<number>,
    y: Subscribable<number>,
    rotation: Subscribable<number>,
}

export class OancAircraftIcon extends DisplayComponent<OancAircraftIconProps> {
    private svgRef = FSComponent.createRef<SVGSVGElement>();

    private readonly transform = MappedSubject.create(this.props.x, this.props.y, this.props.rotation).sub(([x, y, rotation]) => {
        this.svgRef.instance.style.transform = `translate(${x - 45}px, ${y - 39.625}px) rotate(${rotation}deg)`;
    })

    render(): VNode | null {
        return (
            <svg ref={this.svgRef} class="oanc-svg" viewBox="0 0 90 70.25" style="position: absolute; width: 90px; height: 79.25px;" transform={this.transform}>
                <path
                    class="oanc-airplane-shadow"
                    stroke-width={8}
                    stroke-linecap="round"
                    d="M 4 29.5 H 86 m -41 -29.5 v 70.25 m -11.5 -9.75 h 23.5"
                />
                <path
                    class="oanc-airplane"
                    stroke-width={5}
                    stroke-linecap="round"
                    d="M 4 29.5 H 86 m -41 -29.5 v 70.25 m -11.5 -9.75 h 23.5"
                />
            </svg>
        );
    }
}
