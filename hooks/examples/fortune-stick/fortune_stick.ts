import * as fs from 'fs';
import * as readline from 'readline';

// --- Types showcase: interface, enum, union ---
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

// 60 fortunes inspired by traditional temple divination sticks
const FORTUNES: Fortune[] = [
    { level: FortuneLevel.Best, number: 1, poem: '日出扶桑万处明，贵人相逢百事成。', interpretation: '运如旭日东升，万事亨通，贵人相助。' },
    { level: FortuneLevel.Best, number: 9, poem: '风云际会在江中，似有疑心又未通。宽心且看月中桂，正是开花结子时。', interpretation: '风云际会，待时机成熟自然开花结果。' },
    { level: FortuneLevel.Best, number: 17, poem: '明月当空照天涯，行客此时正还家。行人过河不用船，风送归帆到江干。', interpretation: '归家有路，顺风顺水，所求必遂。' },
    { level: FortuneLevel.Good, number: 2, poem: '春来花发满园林，月到天心分外明。万事今朝皆称意，贵人指点有前程。', interpretation: '春暖花开，前程光明，有贵人指点。' },
    { level: FortuneLevel.Good, number: 10, poem: '绿柳苍苍正当时，任君此去作乾坤。勿言此事无凭据，自有亨通造化时。', interpretation: '时机已到，大胆前行，自有亨通之日。' },
    { level: FortuneLevel.Good, number: 18, poem: '金乌渐渐入扶桑，旧事从新却有光。忽有雁声惊梦觉，桃花笑向短长亭。', interpretation: '旧事重提反有转机，前途光明。' },
    { level: FortuneLevel.Fair, number: 3, poem: '天地感应万物苏，凡事心中有定图。平素常存修善德，天时地利自然扶。', interpretation: '平心静气，修养德行，自有天助。' },
    { level: FortuneLevel.Fair, number: 11, poem: '欲求胜事可非常，争奈亲姻日转忙。到头思忖偏疑信，未知此事决然详。', interpretation: '事有变数，需深思熟虑，不可急进。' },
    { level: FortuneLevel.Fair, number: 19, poem: '急水滩头泛轻舟，且喜来到岸边头。过去暗事都不见，舟中又有一轮收。', interpretation: '历经险滩已靠岸，前途渐明，宜守成。' },
    { level: FortuneLevel.Bad, number: 4, poem: '渐渐趋安不必忧，千年枯木又逢秋。且从平地修心德，看取一生何处愁。', interpretation: '运势低回，宜修心养性，静待转机。' },
    { level: FortuneLevel.Bad, number: 12, poem: '东边事出又惊心，宝物沉埋未可寻。恰似猿猴守玉桂，不知何日得升沉。', interpretation: '事多波折，宝物难寻，需耐心等待。' },
    { level: FortuneLevel.Bad, number: 20, poem: '前途变化喜重重，禄马天庭百事通。若问中间迟滞处，重重辛苦任西东。', interpretation: '前途有喜但中间波折，需经历辛苦方成。' },
    { level: FortuneLevel.Worst, number: 5, poem: '云遮月色暗沉沉，凡事谋为未称心。且待云开月朗处，那时渐有好音临。', interpretation: '运势最暗，须待云开月明方有转机。' },
    { level: FortuneLevel.Worst, number: 13, poem: '晨昏祈祷莫嗟伤，必有亨期到眼堂。忧愁变化皆消散，祸退福来禄渐昌。', interpretation: '虔诚祈祷，祸退福来，终有转机。' },
    { level: FortuneLevel.Worst, number: 21, poem: '上下交孚事可成，何须迟疑半途行。天书下诏催科第，禄马天庭百事兴。', interpretation: '事虽可成但须坚定，不可半途而废。' },
    { level: FortuneLevel.Best, number: 6, poem: '福如东海寿南山，君尔何须叹苦烦。宽心且看月中桂，正是开花结子时。', interpretation: '福寿双全，花开结子，大吉大利。' },
    { level: FortuneLevel.Good, number: 14, poem: '宛如仙鹤出樊笼，脱难逃灾去路通。南北东西无阻隔，任君直上九霄宫。', interpretation: '如鹤出笼，前途无阻，大展宏图。' },
    { level: FortuneLevel.Fair, number: 7, poem: '云开月正中，暗事又逢通。凡事多遂意，禄马自然通。', interpretation: '云开月明，暗事转通，凡事顺遂。' },
    { level: FortuneLevel.Fair, number: 15, poem: '上下交孚事可成，何须迟疑半途行。天书下诏催科第，禄马天庭百事兴。', interpretation: '上下同心事可成，不可迟疑。' },
    { level: FortuneLevel.Bad, number: 8, poem: '孤舟得水离浅滩，除去艰难日渐宽。凡事在中还有报，何须迟疑半途间。', interpretation: '离浅滩虽渐宽，但事在中间仍有反复。' },
    { level: FortuneLevel.Worst, number: 16, poem: '事如逐鹿不知足，何曾顷刻心安伏。水边月上遇贵人，方可谋为宜保守。', interpretation: '贪心不足，需遇贵人方可转安。' },
];

// --- async/await showcase: async readline for stdin ---
async function readInit(): Promise<string> {
    const rl = readline.createInterface({ input: process.stdin, crlfDelay: Infinity });
    const firstLine = await new Promise<string>((resolve) => {
        rl.once('line', (line: string) => {
            rl.close();
            resolve(line);
        });
    });
    return firstLine;
}

function pickFortune(): Fortune {
    return FORTUNES[Math.floor(Math.random() * FORTUNES.length)];
}

// --- Template literal showcase ---
function formatFortune(f: Fortune): string {
    return `第${f.number}签 ${f.level}\n${f.poem}\n解曰：${f.interpretation}`;
}

function output(text: string): void {
    const escaped = JSON.stringify(text);
    console.log(`{"type":"output","outputs":[{"kind":"text","text":${escaped}}]}`);
    console.log(`{"type":"done","result":"ok"}`);
}

async function main(): Promise<void> {
    // hook.v1: read only the first line (init frame)
    await readInit();

    const fortune = pickFortune();
    const text = formatFortune(fortune);
    output(text);
}

main().catch((err: Error) => {
    console.error(err);
    console.log(`{"type":"error","error":"${err.message.replace(/"/g, '\\"')}"}`);
});
