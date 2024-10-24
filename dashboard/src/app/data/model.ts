export interface Status {
    version: string;
    comps: number[];
    gauge: number;
    repetitions: number;
    threshold: number;
}

export interface MoveCommand {
    lin: number | undefined;
    wire: number | undefined;
    linStep: number | undefined;
    wireStep: number | undefined;
}

export interface RunCommand {
    pre: number,
    len: number,
    pos: number,
    repetitions: number,
    gauge: number
}

export interface SaveThreshold {
    threshold: number
}