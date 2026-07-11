import * as readline from 'readline';

enum FortuneLevel {
    Best = '上上签',
    Good = '上签',
    Fair = '中签',
    Bad = '下签',
    Worst = '下下签',
}

interface Fortune {
    level: FortuneLevel;
    number: number;
    poem: string;
    interpretation: string;
}

interface Frame {
    type?: string;
    id?: string;
    method?: string;
}

const FORTUNES: Fortune[] = [
    { level: FortuneLevel.Best, number: 1, poem: '日出扶桑万处明，贵人相逢百事成。', interpretation: '运如旭日东升，万事亨通，贵人相助。' },
    { level: FortuneLevel.Best, number: 9, poem: '风云际会在江中，似有疑心又未通。宽心且看月中桂，正是开花结子时。', interpretation: '风云际会，待时机成熟自然开花结果。' },
    { level: FortuneLevel.Good, number: 2, poem: '春来花发满园林，月到天心分外明。万事今朝皆称意，贵人指点有前程。', interpretation: '春暖花开，前程光明，有贵人指点。' },
    { level: FortuneLevel.Fair, number: 3, poem: '天地感应万物苏，凡事心中有定图。平素常存修善德，天时地利自然扶。', interpretation: '平心静气，修养德行，自有天助。' },
    { level: FortuneLevel.Bad, number: 4, poem: '渐渐趋安不必忧，千年枯木又逢秋。且从平地修心德，看取一生何处愁。', interpretation: '运势低回，宜修心养性，静待转机。' },
    { level: FortuneLevel.Worst, number: 5, poem: '云遮月色暗沉沉，凡事谋为未称心。且待云开月朗处，那时渐有好音临。', interpretation: '运势最暗，须待云开月明方有转机。' },
];

function write(frame: object): void {
    process.stdout.write(`${JSON.stringify(frame)}\n`);
}

function formatFortune(fortune: Fortune): string {
    return `第${fortune.number}签 ${fortune.level}\n${fortune.poem}\n解曰：${fortune.interpretation}`;
}

function response(id: string | undefined, result: object): void {
    write({ type: 'response', id, ok: true, result });
}

const input = readline.createInterface({ input: process.stdin, crlfDelay: Infinity });
input.on('line', (line: string) => {
    let frame: Frame;
    try {
        frame = JSON.parse(line) as Frame;
    } catch (error) {
        console.error(error);
        return;
    }
    if (frame.type !== 'request') return;

    if (frame.method === 'system.init') {
        response(frame.id, {});
        return;
    }
    if (frame.method === 'system.shutdown') {
        response(frame.id, {});
        input.close();
        return;
    }
    if (frame.method !== 'event.handle') {
        write({ type: 'response', id: frame.id, ok: false, error: 'unsupported method' });
        input.close();
        return;
    }

    const fortune = FORTUNES[Math.floor(Math.random() * FORTUNES.length)];
    response(frame.id, {
        status: 'completed',
        outputs: [{ kind: 'text', text: formatFortune(fortune) }],
    });
    input.close();
});
